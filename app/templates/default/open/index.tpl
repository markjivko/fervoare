<?php $this->jQuery(array('uniform','tooltip'),'content','admin');?><!doctype html>
<html lang="<?php print lang();?>">
    <head>
        <?php $this->head();?>
    </head>
    <body>
        <a class="skip-link" href="#main-wrapper">Skip to main</a>
        <div id="main-wrapper">
            <div id="top">
                <a href="/" id="logo">Fervoare CMS</a>
                <div id="menu"><?php $this->menu(null);?></div>
            </div>
            <div class="presentation">
                <div class="presentation-grid">
                    <div class="fervoare">
                        <div data-hover="framework"></div>
                        <div data-hover="framework" id="s"></div>
                        <div data-hover="framework" id="s1"></div>
                        <div data-role="dialog" class="framework-dialog">
                            <h1>A tiny footprint</h1>
                            A content management system built on top of <b>OctoMS</b>, a framework with:<ul>
                                <li>A small footprint</li>
                                <li>Self-documentation</li>
                                <li>A strong debugger</li>
                            </ul>
                            <div class="bubble"></div>
                        </div>
                        <div data-hover="task"></div>
                        <div data-role="dialog" class="task-dialog">
                            <h1>Beyond a CMS</h1>
                                An out-of-the-box content editor with authentication, roles and 
                                the full power of a Model-View-Controller framework for your next project
                            <div class="bubble"></div>
                        </div>
                        <div data-hover="cms"></div>
                        <div data-role="dialog" class="cms-dialog">
                            <h1>Easy to set up</h1>
                            Getting started is really easy. All you need to do is create a MySQL database and the installer takes care of the rest.
                            <div class="bubble"></div>
                        </div>
                        <div data-hover="startup"></div>
                        <div data-hover="startup" id="s2"></div>
                        <div data-role="dialog" class="startup-dialog">
                            <h1>Your next Web Startup</h1>
                            "Nothing great was ever achieved without enthusiasm." - Ralph Waldo Emerson<br/><br/>
                            Fervoare (Romanian for "enthusiasm") is not just a framework and a CMS. It't the core of your next Software-as-a-Service.
                            <div class="bubble"></div>
                        </div>
                        <div class="feet-holder"><div class="feet"></div></div>
                    </div>
                </div>
            </div>
            <div class="submenu">
                <ul>
                    <li class="framework">Light</li>
                    <li class="task">Extensible</li>
                    <li class="cms">1-click Install</li>
                    <li class="startup">SaaS ready</li>
                </ul>
            </div>
            <div id="content">
                <?php $this->content();?>
                <?php $this->region('content');?>
            </div>
        </div>
    </body>
</html>