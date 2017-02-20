import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Scanner;

import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JTextArea;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.data.xy.XYSeries;
import org.jfree.data.xy.XYSeriesCollection;
import com.fazecast.jSerialComm.SerialPort;

//part of the code is from the following YouTube tutorial
//https://www.youtube.com/watch?v=cw31L_OwX3A
//JFreeChart is used

public class photogate {

	static SerialPort chosenPort; 
	static int x = 0;
	static int previousNumber = 0;
	static long startTime = 0;
	static long stopTime = 0;
	static double airtime = 0;
	static double jumpHeight = 0;
	
	public static void main(String[] args){
		//create a window
		JFrame window = new JFrame();
		window.setTitle("RealTime Graph");
		window.setSize(700, 450);
		window.setLayout(new BorderLayout());
		window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		JComboBox<String> portList = new JComboBox<String>();	//create a dropdown box
		JButton connectButton = new JButton("Connect");	//create a button
		JButton clearButton = new JButton("Clear");	//this button is for clear the screen
		JPanel topPanel = new JPanel();
		JTextArea textArea = new JTextArea(2, 15);
		textArea.setText("Airtime:\n");
		textArea.append("Jump Height*:");
		topPanel.add(portList);
		topPanel.add(connectButton);
		topPanel.add(clearButton);
		topPanel.add(textArea);
		window.add(topPanel, BorderLayout.NORTH); 

		//populate the drop-down box
		SerialPort [] portNames = SerialPort.getCommPorts();
		for(int i = 0; i < portNames.length; i++){
			portList.addItem(portNames[i].getSystemPortName());	//add the system port to the drop-down box
		}

		//create the realtime graph
		XYSeries series = new XYSeries("*Jump height is calculated without air resistance");	//create a XY coordinate. 
		XYSeriesCollection dataset = new XYSeriesCollection(series);	//create a collection to handle multiple sensors
		JFreeChart chart = ChartFactory.createXYLineChart("Photoresistor Readings", "Time", "Jump Plates", dataset);	//the second parameter represents the lable on x axis
		window.add(new ChartPanel(chart), BorderLayout.CENTER);

		//add the event listener to make the connection button work
		//create another thread to listen to the serial port
		connectButton.addActionListener(new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e){
				if(connectButton.getText().equals("Connect")){
					//connect the serial port
					chosenPort = SerialPort.getCommPort(portList.getSelectedItem().toString());
					chosenPort.setComPortTimeouts(SerialPort.TIMEOUT_SCANNER, 5000, 5000);	//if the port is not connected in 5 seconds, the preogram will skip and let the user to re-choose the port
					//the TIMEOUT_SCANNER allows the program to read text from a serial port
					if(chosenPort.openPort() == true){	//if successfully connected
						connectButton.setText("Disconnect");
						portList.setEnabled(false);	//disable the drop down box; once connection, the port can no longer be changed 
						//create a thread and listen to the serial port


						//the reason why the thread is used here is that the while loop above will never end, and with the thread, 
						//we can still operate on the program without causing it froze
						Thread thread = new Thread(){	//another anonymous class
							@Override
							public void run(){
								Scanner scan = new Scanner(chosenPort.getInputStream());	// in order to connect to the serial port instead of from the keyboard
								long prevTime = 0;
								long currTime = 0;
								while(scan.hasNextLine()){	//read the output from the Arduino
									try{
										String line = scan.nextLine();	//read in the line and convert it into an integer
										int number = Integer.parseInt(line);
										if(previousNumber - number > 120){
											startTime = System.currentTimeMillis();
											prevTime = startTime;
										}
										if(number - previousNumber > 120){
											startTime = System.currentTimeMillis();
											currTime = startTime;
											airtime = (double)Math.abs(prevTime - currTime)/1000;
											jumpHeight = 0.5 * 9.81 * Math.pow(airtime/2, 2);	//formula for calculating the jump height; air resistance ignored
											textArea.setText("Airtime: " + airtime + "s\n");
											textArea.append("Jump Height*: " + String.format("%.3f", jumpHeight) + "m");
										}
										series.add(x++, number);	//to make the graph upside down, do "1023 - number" instead of number;
										previousNumber = number;
										window.repaint();
									}
									catch(Exception e){}
								}
								scan.close();
							}
						};
						thread.start();
					}
				}
				else{
					//disconnect
					chosenPort.closePort();
					portList.setEnabled(true);
					connectButton.setText("Connect");
				}
			}
		});

		clearButton.addActionListener(new ActionListener() {	//if the clear button is hit, both the textbox and the graph will be cleared
			@Override
			public void actionPerformed(ActionEvent e) {
				series.clear();
				x = 0;
				textArea.setText("Airtime:\n");
				textArea.append("Jump Height*:");	//clear the text area
			}
		});
		window.setLocationRelativeTo(null);	//center the window
		window.setVisible(true);

	}
}
