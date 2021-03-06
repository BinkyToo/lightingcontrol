<?php
session_start();

if (!array_key_exists("serialport")){
    $_SESSION["serialport"] = "/dev/ttyACM0";
    }

$filepointer = fopen($_SESSION["serial port"], "r+");

if (isset($_POST["serialPort"])) {
    fclose($filepointer);
    if (in_array($_POST["serialPort"], ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"])){
        $_SESSION["serial port"] = "/dev/ttyACM".$_POST["serialPort"];
    }
    else {
        die("Attempted to use forbidden serial port!");
    }
    $filepointer = fopen($_SESSION["serial port"], "r+");
}

if (isset($_POST["wave"])) {
$wave = $_POST["wave"];
fwrite($filepointer, $wave."\n");
}

elseif (isset($_POST["speed"])) {
$speed = $_POST["speed"];
switch ($speed) {
    case "Max":
        fwrite($filepointer, "fast\n");
    break;
    case "+":
        fwrite($filepointer, "faster\n");
    break;
    case "-":
        fwrite($filepointer, "slower\n");
    break;
    case "Min":
        fwrite($filepointer, "slow\n");
    break;
    default:
        die("Post data error: Not a recognised speed");
    }
}
elseif (isset($_POST["interpolation"])) {
$interpolation = $_POST["interpolation"];
switch ($interpolation) {
    case "On":
        fwrite($filepointer, "interpolation on\n");
    break;
    case "Off":
        fwrite($filepointer, "interpolation off\n");
    break;
    default:
        die("Post data error: Not a recognised interpolation mode");
    }
}
elseif (isset($_POST["status"])) {
$status = $_POST["status"];
switch ($status) {
    case "Get status":
        fwrite($filepointer, "status\n");
    break;
    default:
        die("Post data error: Not a recognised status request");
    }
}
elseif (isset($_POST["controlmode"])) {
$controlmode = $_POST["controlmode"];
fwrite($filepointer, $controlmode."\n");
}
elseif (isset($_POST["channelset"])) {
$channelset = $_POST["channelset"];
fwrite($filepointer, $channelset."\n");
}
else {
    fwrite($filepointer, "ping\n");
}
usleep(50000);
do {
    $newchar = fgetc($filepointer);
    $serialrecieved = $serialrecieved.$newchar;
    usleep(5000);
} while ($newchar !== FALSE);

fclose($filepointer);
?>

<html>
<head>
    <title>Lighting control v2.0</title>
</head>
<body>
    <center>
    <img src="holly.png" width="100">
    <h1>Christmas lighting control panel</h1>
    <div style="width:640px;color:#000000;background-color:#f0f0f0">Currently a work-in-progress</div><br>
        <form method="post" action="<?php echo $PHP_SELF;?>">
            <table border="0">
                <tr><td>
                    Serial Port:
                </td><td>
                    <input type="submit" value="0" name="serialPort" style="width:30px">
                    <input type="submit" value="1" name="serialPort" style="width:30px">
                    <input type="submit" value="2" name="serialPort" style="width:30px">
                    <input type="submit" value="3" name="serialPort" style="width:30px">
                    <input type="submit" value="4" name="serialPort" style="width:30px">
                    <input type="submit" value="5" name="serialPort" style="width:30px">
                    <input type="submit" value="6" name="serialPort" style="width:30px">
                    <input type="submit" value="7" name="serialPort" style="width:30px">
                    <input type="submit" value="8" name="serialPort" style="width:30px">
                    <input type="submit" value="9" name="serialPort" style="width:30px">
                </td></tr>
                <tr><td></td><td>
                    <input type="submit" value="Off" name="wave" style="width:80px">
                    <input type="submit" value="On" name="wave" style="width:80px">
                </td></tr>
                <tr><td>
                    Waves
                    </td><td>
                        <input type="submit" value="Sine" name="wave" style="width:80px">
                        <input type="submit" value="Square" name="wave" style="width:80px">
                        <input type="submit" value="Triangle" name="wave" style="width:80px">
                        <input type="submit" value="Saw" name="wave" style="width:80px">
                        <input type="submit" value="waS" name="wave" style="width:80px">
                </td></tr><tr><td></td><td>
                        <input type="submit" value="Short Blip" name="wave" style="width:101px">
                        <input type="submit" value="Long Blip" name="wave" style="width:101px">
                        <input type="submit" value="Short Unblip" name="wave" style="width:101px">
                        <input type="submit" value="Long Unblip" name="wave" style="width:101px">
                </td></tr><tr><td>
                    Speed
                    </td><td>
                    <input type="submit" value="Min" name="speed" style="width:38px">
                    <input type="submit" value="-" name="speed" style="width:38px">
                    <input type="submit" value="+" name="speed" style="width:38px">
                    <input type="submit" value="Max" name="speed" style="width:38px">
                </td></tr><tr><td>
                    Interpolation
                    </td><td>
                    <input type="submit" value="Off" name="interpolation" style="width:38px">
                    <input type="submit" value="On" name="interpolation" style="width:38px">
                </td></tr><tr><td>
                    </td><td>
                    <input type="submit" value="Get status" name="status" style="width:80px">
                </td></tr><tr><td>
                    </td><td>
                    <input type="submit" value="Manual" name="controlmode" style="width:80px">
                    <input type="submit" value="Auto" name="controlmode" style="width:80px">
                </td></tr>
                <tr><td>
                    </td><td>
                    <input type="submit" value="Red On" name="channelset" style="width:80px">
                    <input type="submit" value="Yellow On" name="channelset" style="width:80px">
                    <input type="submit" value="Green On" name="channelset" style="width:80px">
                </td></tr>
                <tr><td>
                    </td><td>
                    <input type="submit" value="Red Off" name="channelset" style="width:80px">
                    <input type="submit" value="Yellow Off" name="channelset" style="width:80px">
                    <input type="submit" value="Green Off" name="channelset" style="width:80px">
                </td></tr>
            </table>
        </form>
    <?php if ($serialrecieved == False) {
            echo('<div style="width:640px;color:#000000;background-color:#Ff8080">Arduino is not responding!</div>');
        }
        else {
            echo('<div style="width:640px;color:#000000;background-color:#90ff90">Arduino is responding.</div>');
            echo('<font face="monospace">'.$serialrecieved.'</font>');
        }
    ?>
    <br><br>
    <img src="holly-flip.png" width="100">
    </center>
</body>
</html>