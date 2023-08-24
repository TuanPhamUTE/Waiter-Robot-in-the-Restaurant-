<?php
    // send a JSON message to website
    header("Content-Type: application/json");

    // connect to databse
    include("config.php");

    // Read data from database
    $sql = "select *from RPM_VEL where stt = (select max(stt) from RPM_VEL)";
    $result = mysqli_query($conn, $sql);
    
    // Send data to website 
    $data = array();
    foreach($result as $row){
        $data[] = $row;
    }
    mysqli_close($conn);

    echo json_encode($data);
?>