<!DOCTYPE html>
<html  xmlns="http://www.w3.org/1999/xhtml" dir="ltr" lang="<?php print lang();?>" lang="<?php print lang();?>">
    <head>
        <?php $this->head();?>
    </head>
    <body class="login">
        <div id="main-wrapper">
            <div id="login">
            <div id="logo"><a href="https://github.com/markjivko/fervoare" target="_blank" title="Fervoare CMS: <?php print t2(__FILE__,2,'Created by Mark Jivko');?>">Fervoare CMS</a></div>
            <?php print form(null,null,array('class'=>'x','rel'=>'plain'));
                print label(t2(__FILE__,3,'Username'),'username').input(null, 'username', null, array('autocomplete' => 'username')).tag('br',null,null,true);
                print label(t2(__FILE__,4,'Password'),'password').input('password', 'password', null, array('autocomplete' => 'current-password')).tag('br',null,null,true);
                print submit(t2(__FILE__,5,'Log In'));
                $this->region('error');
                print form(true);
            ?>
            </div>
        </div>
    </body>
</html>