/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package e.mail;

import static e.mail.EMail.clientSocket;
import javafx.geometry.Insets;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.URL;
import java.util.Optional;
import java.util.ResourceBundle;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;
import javafx.event.Event;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonBar.ButtonData;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.PasswordField;
import javafx.scene.control.TextField;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;
import javafx.stage.Window;
import javafx.util.Pair;

/**
 * FXML Controller class
 *
 * @author apasi
 */
public class EmailFXMLController implements Initializable {

    public static Mail mailDetails;
    public static Contact contactDetails;
    
    @FXML
    private Label label1;
    
    @FXML
    private Label label2;
    
    @FXML
    private Button sendRecieveButton;
    
    @FXML
    private ListView<Mail> recievedL = new ListView<>();
    
    @FXML private void handleRecieved(Event event){
        try {
            mailDetails = recievedL.getSelectionModel().getSelectedItem();
            FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("ShowMail.fxml"));
            Parent root1 = (Parent) fxmlLoader.load();
            Stage stage = new Stage();
            stage.setScene(new Scene(root1));  
            stage.show();
        } catch(Exception e) {
            e.printStackTrace();
          }
    }
    
    @FXML
    private Label label3;
    
    @FXML
    private ListView<Contact> contactsL = new ListView<>();
    
    @FXML private void handleContacts(Event event){
        try {
            contactDetails = contactsL.getSelectionModel().getSelectedItem();
            FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("ContactDetails.fxml"));
            Parent root1 = (Parent) fxmlLoader.load();
            Stage stage = new Stage();
            stage.setScene(new Scene(root1));  
            stage.show();
        } catch(Exception e) {
            e.printStackTrace();
          }
    }
    
    @FXML
    private Label label4;
    
    @FXML
    private ListView<Mail> sendL = new ListView<>();
    
    @FXML private void handleSend(Event event){
        try {
            mailDetails = sendL.getSelectionModel().getSelectedItem();
            FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("ShowMail.fxml"));
            Parent root1 = (Parent) fxmlLoader.load();
            Stage stage = new Stage();
            stage.setScene(new Scene(root1));  
            stage.show();
        } catch(Exception e) {
            e.printStackTrace();
          }
    }
    
    @FXML private void handleNewMessage(Event event){
        try {
            contactDetails = contactsL.getSelectionModel().getSelectedItem();
            FXMLLoader fxmlLoader = new FXMLLoader(getClass().getResource("NewMessage.fxml"));
            Parent root1 = (Parent) fxmlLoader.load();
            Stage stage = new Stage();
            stage.setScene(new Scene(root1));  
            stage.show();
        } catch(Exception e) {
            e.printStackTrace();
          }
    }
    //TODO - Odbieranie Maili
    @FXML private void handleRecieve(Event event) throws IOException{
        //TODO - recieve by socket
        Window owner = sendRecieveButton.getScene().getWindow();
        byte[] buffer = new byte[1000];
        try {
            InputStream is = clientSocket.getInputStream();
            is.read(buffer);
            System.out.println(new String(buffer, "US-ASCII"));
            String fullMsg = new String(buffer, "US-ASCII");
            //TODO create email
            //M - message - 0
            //odbiorca - 1
            //nadawca - 2
            //topic - 3
            //message - 4
            String[] parts = fullMsg.split("/");
            Mail newMail = new Mail(parts[2], parts[3], parts[4]);
            EMail.recievedList.add(newMail);
        } catch (IOException ex) {
            Logger.getLogger(EmailFXMLController.class.getName()).log(Level.SEVERE, null, ex);
            AlertHelper.showAlert(Alert.AlertType.ERROR, owner, "Error!", "Blad - sprobuj ponownie !");
        }
	
    }
    
    @FXML private void handleNewContact(Event event){
        // Create the custom dialog.
        Dialog<Pair<String, String>> dialog = new Dialog<>();
        dialog.setTitle("New Contact");
        dialog.setHeaderText("Create new contact");

        // Set the icon (must be included in the project).
        //dialog.setGraphic(new ImageView(this.getClass().getResource("login.png").toString()));

        // Set the button types.
        ButtonType loginButtonType = new ButtonType("Create", ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(loginButtonType, ButtonType.CANCEL);

        // Create the username and password labels and fields.
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextField name = new TextField();
        name.setPromptText("Name");
        TextField address = new TextField();
        address.setPromptText("Address");

        grid.add(new Label("Name:"), 0, 0);
        grid.add(name, 1, 0);
        grid.add(new Label("Address:"), 0, 1);
        grid.add(address, 1, 1);

        // Enable/Disable login button depending on whether a username was entered.
        Node loginButton = dialog.getDialogPane().lookupButton(loginButtonType);
        loginButton.setDisable(true);

        // Do some validation (using the Java 8 lambda syntax).
        name.textProperty().addListener((observable, oldValue, newValue) -> {
            loginButton.setDisable(newValue.trim().isEmpty());
        });

        dialog.getDialogPane().setContent(grid);

        // Request focus on the username field by default.
        Platform.runLater(() -> name.requestFocus());

        // Convert the result to a username-password-pair when the login button is clicked.
        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == loginButtonType) {
                return new Pair<>(name.getText(), address.getText());
            }
            return null;
        });

        Optional<Pair<String, String>> result = dialog.showAndWait();
        
        result.ifPresent(usernamePassword -> {
            EMail.contactList.add(new Contact(usernamePassword.getKey(), usernamePassword.getValue()));
            
        });
    }
    
    @Override
    public void initialize(URL url, ResourceBundle rb) {
        Dialog<Pair<String, String>> dialog = new Dialog<>();
        dialog.setTitle("Login");
        dialog.setHeaderText("Log into your account: ");

        // Set the icon (must be included in the project).
        //dialog.setGraphic(new ImageView(this.getClass().getResource("login.png").toString()));

        // Set the button types.
        ButtonType loginButtonType = new ButtonType("Log in", ButtonData.OK_DONE);
        dialog.getDialogPane().getButtonTypes().addAll(loginButtonType, ButtonType.CANCEL);

        // Create the username and password labels and fields.
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(20, 150, 10, 10));

        TextField address = new TextField();
        address.setPromptText("Address");
        PasswordField pass = new PasswordField();
        pass.setPromptText("Password");

        grid.add(new Label("Name:"), 0, 0);
        grid.add(address, 1, 0);
        grid.add(new Label("Address:"), 0, 1);
        grid.add(pass, 1, 1);

        // Enable/Disable login button depending on whether a username was entered.
        Node loginButton = dialog.getDialogPane().lookupButton(loginButtonType);
        loginButton.setDisable(true);

        // Do some validation (using the Java 8 lambda syntax).
        address.textProperty().addListener((observable, oldValue, newValue) -> {
            loginButton.setDisable(newValue.trim().isEmpty());
        });

        dialog.getDialogPane().setContent(grid);

        // Request focus on the username field by default.
        Platform.runLater(() -> address.requestFocus());

        // Convert the result to a username-password-pair when the login button is clicked.
        dialog.setResultConverter(dialogButton -> {
            if (dialogButton == loginButtonType) {
                return new Pair<>(address.getText(), pass.getText());
            }
            return null;
        });

        Optional<Pair<String, String>> result = dialog.showAndWait();
        
        result.ifPresent(usernamePassword -> {
            //TODO - login via socket
            //L - login
            //nick - toSend
            try {
                
                OutputStream os = clientSocket.getOutputStream();
                EMail.loggedUser = address.getText();
                String msg = "L/"+address.getText()+"/";
                os.write(msg.getBytes());
                //EMail.contactList.add(new Contact(usernamePassword.getKey(), usernamePassword.getValue()));
            } catch (IOException ex) {
                Logger.getLogger(EmailFXMLController.class.getName()).log(Level.SEVERE, null, ex);
            }
            
        });
        recievedL.setItems(EMail.recievedList);
        sendL.setItems(EMail.sendList);
        contactsL.setItems(EMail.contactList);
    }    
    
}
