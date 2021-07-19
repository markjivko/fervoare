<h1><?php print t(__FILE__,1,"Edit the site's menus");?></h1>
<?php print action_btn(t(__FILE__,2,'Add a new menu'),true,'*add',array('class'=>'new_menu')); ?>
<?php print form(null,null,array('class'=>'x'));?>
<table class="menu-table">
    <tr>
        <td class="first">
            <div id="menus">
                <?php print $this->v('m');?>    
            </div>
        </td>
        <td>
            <div id="posts">
                <?php print $this->v('p');?>
            </div>
        </td>
        <td>
            <div id="links">
                <?php print $this->v('l');?>
            </div>
        </td>
    </tr>
</table>
<?php print form(true);?>
<help title="<?php print t(__FILE__,8,'Menu help');?>">
<?php print t(__FILE__,9,'<h1>How does it work?</h1>
    <p>You can use the menus defined here in your site\'s template files by calling <code>$this->menu(\'menu_name\');</code><br/>See more on the application library "template" method "menu".</p>
<h1>How to create a new menu</h1>
    <p>Press the "create a new menu" button on top of the page.</p>
<h1>How to add an item to a menu</h1>
    <p>Drag and drop an item from one of the lists on the right into your menu.</p>
<h1>How to remove a menu</h1>
    <p>Press the x mark on the top right of the menu you want to remove.</p>
<h1>How to remove a menu item</h1>
    <p>Drag and drop the item back into one of the lists on the right.</p>
<h1>How to rename a menu</h1>
    <ul>
    <li>Click on the menu\'s name and it will become editable.</li>
    <li>Change the name to whatever you want.</li>
    <li>Click anywhere outside the menu\'s name to preserve the change.</li>
    </ul>
<h1>How to rename a menu item</h1>
    <ul>
    <li>Click on the menu item\'s name and it will become editable.</li>
    <li>Change the name to whatever you want.</li>
    <li>Click anywhere outside the menu item\'s name to preserve the change.</li>
    </ul>');?>
</help>