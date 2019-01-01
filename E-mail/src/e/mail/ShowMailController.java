/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package e.mail;

import java.net.URL;
import java.util.ResourceBundle;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Label;

/**
 * FXML Controller class
 *
 * @author apasi
 */
public class ShowMailController implements Initializable {
    
    @FXML
    private Label title = new Label();
    
    @FXML
    private Label sender = new Label();
    
    @FXML
    private Label mail = new Label();
    
    
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        sender.setText(EmailFXMLController.mailDetails.getAdress());
        mail.setText(EmailFXMLController.mailDetails.getMessage());
    }    
    
}
