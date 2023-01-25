<?php

session_start();

if(isset($_POST["username"]))
{
    $_SESSION["is_login"] = 1;
    $_SESSION["name"] = $_POST["username"];
}
 ?>
<!DOCTYPE html>
<html lang="en" >
<head>
    <meta charset="UTF-8">
    <title>HTML5 Login Form with validation Example</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/normalize/5.0.0/normalize.min.css">
    <link rel="stylesheet" href="/bonus/php/style.css">
</head>
<body onLoad="onPageLoad()">

<div id="login-form-wrap">
    <?php if(!isset($_SESSION["is_login"]) || $_SESSION["is_login"] == 0)  { ?>
        <h2>Login</h2>
                  <form id="login-form" method="POST" action="/bonus/php/index.php">
                          <input type="text" id="username" name="username" placeholder="Username" required><i class="validation"><span></span><span></span></i>
                          <input type="submit" id="login" value="Login">
                  </form>
                  <div id="create-account-wrap">
                      <p>Not a member? <a href="#">Create Account</a><p>
                  </div>


    <?php } else  { ?>
        <h2>Welcome <?php echo $_SESSION["name"] ?> </h2>
        <label>Theme</label>
                        <select name="theme" id="theme" onChange="setTheme(this.value)">

                                   <option value="1" <?php echo $_COOKIE['theme'] == 1 ? 'selected' : '' ?> >Light</option>
                                   <option value="2" <?php echo $_COOKIE['theme'] == 2 ? 'selected' : '' ?> >Dark</option>
                                 </select>

    <?php } ?>

 </div>

    <script src="/bonus/php/cookie.js"></script>

</body>
</html>
