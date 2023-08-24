<?php
    // connect to database 
    $server = "localhost"; // IP addressof server
    $user = "tuanpham";
    $pass = "016365";
    $database = "FINAL_EMBEDDED_IOT";
    
    $conn = mysqli_connect($server,$user,$pass,$database);

    // Check communication 
    if(mysqli_connect_errno()){
        //in bao loi len trang web
        echo "Failed to connect to DB: " . mysqli_connect_error();
        exit();
    }

?>