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
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;

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
    
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        // TODO
    }    
    
}
