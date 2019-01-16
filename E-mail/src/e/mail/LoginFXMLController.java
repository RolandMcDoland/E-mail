/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package e.mail;

import static e.mail.EMail.clientSocket;
import static e.mail.EmailFXMLController.contactDetails;
import java.io.IOException;
import java.io.OutputStream;
import java.net.URL;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.event.Event;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

/**
 * FXML Controller class
 *
 * @author apasi
 */
public class LoginFXMLController implements Initializable {
    
    
    @FXML
    private Button log;
    
    @FXML
    private Button cancel;
    
    @FXML
    private TextField input;
    
    @FXML private void handleCancel(Event event) throws IOException{
        Stage stage = (Stage) cancel.getScene().getWindow();
        // do what you have to do
        stage.close();
    }
    
    @FXML private void handleLog(Event event) throws IOException{
        String login = input.getText();
        if(login != null && !login.isEmpty()) {
        try {
            OutputStream os = clientSocket.getOutputStream();
            EMail.loggedUser = login;
            String msg = "L/"+ login+"/";
            os.write(msg.getBytes());
        } catch (IOException ex) {
            Logger.getLogger(EmailFXMLController.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("EmailFXML.fxml"));
            Parent root1 = (Parent) fxmlLoader.load();
            Stage stage = new Stage();
            stage.setScene(new Scene(root1));  
            stage.show();
        } catch(Exception e) {
            e.printStackTrace();
          }
        Stage stage = (Stage) log.getScene().getWindow();
        // do what you have to do
        stage.close();
        }
        else{
            
        }
    }
    
    

    /**
     * Initializes the controller class.
     */
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        input.setPromptText("Address");
        
        // TODO
    }    
    
}
