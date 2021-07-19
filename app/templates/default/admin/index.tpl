<!doctype html>
<html dir="ltr" lang="<?php print lang();?>">
    <head>
        <?php $this->head();?>
        <script type="text/javascript" async defer>var img = "<?php print $this->url('img',array('tpl'=>false));?>";</script>
    </head>
    <body>
        <div id="main-wrapper">
            <div id="top">
                <a href="/" id="logo">Fervoare CMS</a>
                <?php $this->region('addons');?>
                <div id="gear"><?php print form().lang_select().input('hidden','logout',true).submit(t2(__FILE__,1,'Log Out')).form(true);?></div>
            </div>
            <div id="top-clear"><div id="notif"></div></div>
            <div id="sidebar">
                <?php $this->region('methods');?><div id="copy"><?php $this->user();?><br/><?php $this->powered();?></div>
            </div>
            <div id="content"><?php $this->content();?>
                <div class="history">
                    <div>
                        <a href="javascript:history.back();" class="left-button">Back</a>
                        <p class="center-button"></p><p class="breadcrumbs"><?php $this->breadcrumbs('');?></p>
                        <p class="help-button"></p>
                        <a class="refresh-button">Refresh</a>
                        <a href="javascript:history.forward();" class="right-button">Forward</a>
                    </div>
                </div>
            </div>
        </div>
    </body>
</html>