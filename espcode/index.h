const char configure[] PROGMEM = R"=====(<!doctype html>
<html lang='en'>
<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>Device Configuration</title>
    <style>
        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
        }
        body {
            font-family: 'Segoe UI', Roboto, 'Helvetica Neue', Arial, sans-serif;
            background-color: #f0f4f8;
            color: #333;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .form-container {
            background-color: #fff;
            padding: 2rem;
            border-radius: 10px;
            box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
            width: 100%;
            max-width: 350px;
        }
        .form-container h1 {
            text-align: center;
            font-size: 1.8rem;
            margin-bottom: 1rem;
            color: #007bff;
        }
        .form-container label {
            font-size: 0.9rem;
            color: #555;
            margin-bottom: 0.5rem;
            display: block;
        }
        .form-control {
            width: 100%;
            padding: 0.75rem;
            font-size: 1rem;
            border: 1px solid #ddd;
            border-radius: 5px;
            margin-bottom: 1rem;
            transition: border-color 0.2s;
        }
        .form-control:focus {
            border-color: #007bff;
            outline: none;
            box-shadow: 0 0 4px rgba(0, 123, 255, 0.5);
        }
        button {
            width: 100%;
            padding: 0.75rem;
            font-size: 1rem;
            color: #fff;
            background-color: #007bff;
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.2s;
        }
        button:hover {
            background-color: #0056b3;
        }
        button:active {
            background-color: #004494;
        }
    </style>
</head>
<body>
    <div class="form-container">
        <h1>Wi-Fi Configuration</h1>
        <form action="/" method="post">
            <label for="ssid">WiFi SSID</label>
            <input type="text" id="ssid" class="form-control" name="ssid" placeholder="Enter your WiFi name" required>
            <label for="password">WiFi Password</label>
            <input type="password" id="password" class="form-control" name="password" placeholder="Enter your WiFi password" required>
            <button type="submit">Save Configuration</button>
        </form>
    </div>
</body>
</html>)=====";

const char HTML_OK[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Configuration</title>
  <style>
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      background-color: #f0f4f8;
      color: #333;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
    }
    .message-container {
      background-color: #fff;
      padding: 2rem;
      border-radius: 10px;
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
      text-align: center;
      width: 100%;
      max-width: 300px;
    }
    .message-container h1 {
      font-size: 1.5rem;
      color: #007bff;
    }
  </style>
</head>
<body>
  <div class='message-container'>
    <h1>Configuration Saved. Restarting...</h1>
  </div>
</body>
</html>
)rawliteral";

const char HTML_ERROR[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang='en'>
<head>
  <meta charset='UTF-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1.0'>
  <title>Invalid Parameters</title>
  <style>
    body {
      font-family: 'Segoe UI', Arial, sans-serif;
      background-color: #f0f4f8;
      color: #333;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
      margin: 0;
    }
    .message-container {
      background-color: #fff;
      padding: 2rem;
      border-radius: 10px;
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
      text-align: center;
      width: 100%;
      max-width: 300px;
    }
    .message-container h1 {
      font-size: 1.5rem;
      color: #ff0000;
    }
  </style>
</head>
<body>
  <div class='message-container'>
    <h1>Invalid Parameters</h1>
  </div>
</body>
</html>
)rawliteral";