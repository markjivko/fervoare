<?php $this->jQuery(array('uniform','tooltip'),'content','admin');?>
<!DOCTYPE html>
<html  xmlns="http://www.w3.org/1999/xhtml" dir="ltr" lang="en">
    <head>
        <?php $this->head();?>
    </head>
    <body>
        <div id="main-wrapper">
            <div id="top">
                <a href="<?php print i18n_baseurl();?>" id="logo">Fervoare CMS</a>
                <div id="menu"><?php $this->menu();?></div>
            </div>
            <div class="presentation-page">
                <div class="main">
                    <div class="content">
                        <h1>
                            <?php 
                                echo a(
                                    octoms('settings', OMS_L)->get('seo.blog_title'),
                                    '/' . octoms('settings', OMS_L)->get('seo.blog_slug')
                                );
                            ?> 
                            &#xbb; <?php $this->title();?>
                        </h1>
                        <div class="clear"></div>
                        <br/>
                        <?php $this->content();?>
                    </div>
                </div>
            </div>
            <div class="submenu-page">
                <ul><li class="framework"></li><li class="task"></li><li class="cms"></li><li class="startup"></li></ul>
                <div class="footer">
                    <div class="footer-left">
                        <?php $this->region('footer');?>
                    </div>
                    <div class="footer-right">
                        <?php $this->powered();?>
                    </div>
                </div>
            </div>
        </div>
    
    </body>
</html>