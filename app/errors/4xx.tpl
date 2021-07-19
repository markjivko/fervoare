<?php 
    $root = (WEBROOT!=''?WEBROOT:'').WS.APP_FOLDER.WS.'errors';
    $template = octoms('template',OMS_L);
    $template->js(array(WIZARD.WS.'js'.WS.'common'.WS.'jquery.js','custom'=>true));
    $template->jQuery('uniform','content','admin');
    $template->js(array(sprintf("$(document).ready(function(){\n%s\n});",$template->_get1('__jq__')),'remote'=>false));
?><!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" dir="ltr" lang="en">
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
        <meta name="generator" content="Fervoare CMS" />
        <meta name="author" content="Mark Jivko" />
        <meta name="copyright" content="Fervoare CMS" />
        <meta name="robots" content="noindex,nofollow" />
        <?php $template->js()->css();?>
        <link rel="stylesheet" type="text/css" href="<?php echo $root;?>/css/style.css" />
        <link rel="icon" href="<?php echo $root;?>/img/favicon.png">
            <title><?php echo t2(__FILE__,1,'Error').' '.$info->no;?> - <?php echo $info->errors[$info->no];?></title>
    </head>
    <body>
        <div id="main-wrapper">
            <div id="error">
                <div id="logo"><a href="/" target="_blank" title="Fervoare CMS">Fervoare CMS</a></div>
                <h1><?php echo t2(__FILE__,1,'Error').' '.$info->no;?> - <?php echo $info->errors[$info->no];?></h1>
                <hr/>
                <?php print t2(__FILE__,2,'The page you are looking for does not exist or is not accessible.');?><br/><?php echo $info->message;?>
                <hr/>
                <?php 
                    print link_btn('&#xab; ' . t2(__FILE__, 3, 'Go Back'), 'javascript:history.back();', false);
                    print link_btn('&#x1F3E0; ' . t2(__FILE__, 4, 'Go Home'), i18n_baseurl(), false);
                ?>
            </div>
        </div>
    </body>
</html>