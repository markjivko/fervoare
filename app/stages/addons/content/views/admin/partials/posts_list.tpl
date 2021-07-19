<h1><?php print t(__FILE__,3,'Edit the pages on your site');?></h1>
<?php if (is_array($this->v('d'))):?>
<?php print link_btn(t(__FILE__, 21, 'Add a new post'), 'add');?>
<table class="data">
<thead>
    <tr>
        <th><?php print t(__FILE__,4,'Title');?></th>
        <th><?php print t(__FILE__,5,'Type');?></th>
        <th><?php print t(__FILE__,6,'Status');?></th>
        <th><?php print t(__FILE__,7,'Actions');?></th>
    </tr>
</thead>
<tbody>
    <?php foreach ($this->v('d') AS $page):?>
    <tr>
        <td><?php print $page['title'];?></td>
        <td><?php print $page['type']==0?t(__FILE__, 23, 'Article'):t(__FILE__, 24, 'Blog post');?></td>
        <td><?php print $page['status']==0?t(__FILE__, 25, 'Draft'):t(__FILE__, 26, 'Published');?></td>
        <td width=200 align=center>
        <?php 
            print link_btn(t(__FILE__, 15, 'Edit'),'edit/' .$page['id']);
            print action_btn(t(__FILE__, 16, 'Delete'),$page['id'],'delete',array('confirm'=>t(__FILE__, 20, 'Are you sure you want to delete this post?')));
        ?>
        </td>
    </tr>
    <?php endforeach;?>
</tbody>
</table>
<div class="clear"></div>
<?php else:?>
    <?php print link_btn(t(__FILE__, 22, 'Add your first post'),'add');?>
<?php endif; ?>