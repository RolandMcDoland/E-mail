/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package e.mail;

import static e.mail.EMail.clientSocket;
import java.io.IOException;
import java.io.OutputStream;
import java.net.URL;
import java.util.ResourceBundle;
import javafx.event.Event;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * FXML Controller class
 *
 * @author apasi
 */
public class NewMessageController implements Initializable {

    /**
     * Initializes the controller class.
     */
    @FXML 
    private TextField address;
    
    @FXML 
    private TextField topic;
    
    @FXML
    private TextArea textField;
    
    @FXML
    private Button send;
    
    @FXML
    private Button cancel;
    
    @FXML private void handleSend(Event event) throws IOException{
        Mail mail = new Mail(address.getText(),topic.getText(),textField.getText());
        EMail.sendList.add(mail);
        //TODO - send by socket
        //M - wiadomosc
        OutputStream os = clientSocket.getOutputStream();
	String msg = "M^" + mail.getFullMsg();
	os.write(msg.getBytes());
    }
    
    @FXML private void handleCancel(Event event) throws IOException{
        // get a handle to the stage
        Stage stage = (Stage) cancel.getScene().getWindow();
        // do what you have to do
        stage.close();
    }
    
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        // TODO
    }    
    
}
