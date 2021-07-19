<?php if (count($this->v('r'))>0):?>
    <ul>
        <?php foreach($this->v('r') AS $row):?>
        <li <?php if($row['selected']) print 'class=current';?> title="<?php print htmlspecialchars($row['description'],ENT_QUOTES);?>"><a href="<?php print i18n_baseurl('admin'.WS.$row['slug']);?>"><?php print $row['name'];?></a></li>
        <?php endforeach;?>
    </ul>
<?php endif;?>