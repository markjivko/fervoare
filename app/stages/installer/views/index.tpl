<?php 
    $root = (WEBROOT!=WS?WEBROOT:'').WS.APP_FOLDER.WS.'stages'.WS.'installer'.WS.'views';
?><!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" lang="en-EN">
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <meta name="generator" content="Fervoare CMS https://github.com/markjivko/fervoare" />
        <meta name="author" content="Mark Jivko" />
        <meta name="copyright" content="Fervoare CMS" />
        <meta name="robots" content="noindex,nofollow" />
        <meta name="description" content="Stephino CMS Installer">
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" type="text/css" href="<?php echo $root;?>/css/style.css" />
        <?php octoms::$oms_objects['template_l']->css()->js();?>
        <link rel="icon" href="<?php echo $root;?>/img/favicon.png">
            <title>Welcome to the Fervoare CMS</title>
    </head>
    <body>
        <div id="main-wrapper">
            <div id="install">
                <div id="logo"><a href="/" target="_blank" title="Fervoare CMS">Fervoare CMS</a></div>
                <?php octoms::$oms_objects['template_l']->region('error');?>
                <?php
                    print 
                        form().
                        label('Admin Username','admin_username').input('text','admin_username').
                        label('Admin Password','admin_password').input('password','admin_password').
                        label('MySQL Server','db_server').input('text','db_server','localhost').
                        label('MySQL Username','db_username').input('text','db_username').
                        label('MySQL Password','db_password').input('password','db_password').
                        label('MySQL Database','db_data').input('text','db_data').
                        submit('Install').
                        form(true);
                ?>
            </div>
        </div>
    </body>
</html>