<?php
$serialPort = "/dev/ttyACM7";

$filepointer = fopen($serialPort, "r+");

if (isset($_POST["wave"])) {
$wave = $_POST["wave"];

switch ($wave) {
    case "Off":
    fwrite($filepointer, "off\n");
    break;
    case "On":
    fwrite($filepointer, "on\n");
    break;
    case "Sine":
    fwrite($filepointer, "sine\n");
    break;
    case "Square":
    fwrite($filepointer, "square\n");
    break;
    case "Triangle":
    fwrite($filepointer, "triangle\n");
    break;
    case "Sawtooth":
    fwrite($filepointer, "saw\n");
    break;
    default:
        die("Post data error: Not a recognised wave");
    }
}
elseif (isset($_POST["speed"])) {
$speed = $_POST["speed"];
switch ($speed) {
    case "+":
    fwrite($filepointer, "faster\n");
    break;
    case "-":
    fwrite($filepointer, "slower\n");
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
    <div style="width:500px;color:#000000;background-color:#f0f0f0">Currently a work-in-progress</div><br>
        <form method="post" action="<?php echo $PHP_SELF;?>">
            <table border="0">
                <tr><td></td><td>
                    <input type="submit" value="Off" name="wave" style="width:80px">
                </td></tr><tr><td></td><td>
                        <input type="submit" value="On" name="wave" style="width:80px">
                </td></tr><tr><td>
                    Waves
                    </td><td>
                        <input type="submit" value="Sine" name="wave" style="width:80px">
                        <input type="submit" value="Square" name="wave" style="width:80px">
                        <input type="submit" value="Triangle" name="wave" style="width:80px">
                        <input type="submit" value="Sawtooth" name="wave" style="width:80px">
                </td></tr><tr><td>
                    Speed
                    </td><td>
                    <input type="submit" value="-" name="speed" style="width:38px">
                    <input type="submit" value="+" name="speed" style="width:38px">
                </td></tr><tr><td>
                    Interpolation
                    </td><td>
                    <input type="submit" value="Off" name="interpolation" style="width:38px">
                    <input type="submit" value="On" name="interpolation" style="width:38px">
                </td></tr>
            </table>
        </form>
    <?php if ($serialrecieved == False) {
            echo('<div style="width:500px;color:#000000;background-color:#Ff8080">Arduino is not responding!</div>');
        }
        else {
            echo('<div style="width:500px;color:#000000;background-color:#90ff90">Arduino is responding.</div>');
            echo($serialrecieved);
        }
    ?>
    <br><br>
    <img src="holly-flip.png" width="100">
    </center>
</body>
</html>