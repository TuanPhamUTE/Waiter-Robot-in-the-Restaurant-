<?php
    // connect to database 
    include("config.php");
    $option  = $_POST["textFood"]; 
    $amount =  $_POST["numberAmount"];
    
    $sql = "insert into User12Press (food , amount) values ('$option','$amount')";
 
    mysqli_query($conn,$sql);
    if(!mysqli_query($conn,$sql)){
        echo " Error: ", mysqli_error($conn);
    }
    mysqli_close($conn);

?>