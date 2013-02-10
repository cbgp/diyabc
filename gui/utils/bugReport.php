<?php
$ip=$_SERVER['REMOTE_ADDR'];
$date = date('Y m d H:i:s');
$filename = $date." ".$ip.'.tar.gz';
$somecontent = $_POST["file"];

    if (!$handle = fopen($filename, 'c')) {
         echo "OP";
         exit;
    }
    
    if (fwrite($handle, $somecontent) === FALSE) {
        echo "WP";
        exit;
    }
    
    echo "OK";

    fclose($handle);
?>

