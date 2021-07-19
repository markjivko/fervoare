<h1><?php print t(__FILE__, 14, 'Edit your link collection');?></h1>
<?php if (is_array($this->v('d'))):?>
<?php print link_btn(t(__FILE__,18,'Add a new link'),'add');?>
<table class="data">
<thead>
    <tr>
        <th><?php print t(__FILE__,11,'Title');?></th>
        <th>Href</th>
        <th><?php print t(__FILE__, 7, 'Actions');?></th>
    </tr>
</thead>
<tbody>
    <?php foreach ($this->v('d') AS $link):?>
    <tr>
        <td><?php print $link['title'];?></td>
        <td><?php print $link['href'];?></td>
        <td width=150 align=center>
        <?php 
            print link_btn(t(__FILE__, 15, 'Edit'),'edit/' .$link['id']);
            print action_btn(t(__FILE__, 16, 'Delete'),$link['id'],'delete',array('confirm'=>t(__FILE__, 17, 'Are you sure you want to delete this link?')));
        ?>
        </td>
    </tr>
    <?php endforeach;?>
</tbody>
</table>
<div class="clear"></div>
<?php else:?>
<?php print link_btn(t(__FILE__,19,'Add your first link'),'add');?>
<?php endif; ?>