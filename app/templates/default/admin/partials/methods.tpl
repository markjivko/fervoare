<?php if(is_array($this->v('r')) && count($this->v('r')) > 0):?>
    <ul>
        <?php foreach($this->v('r') AS $slug => $row):?>
        <li <?php if($row['selected']) print 'class=current';?> title="<?php print htmlspecialchars($row['description'],ENT_QUOTES);?>">
            <?php echo a($row['name'], i18n_baseurl('admin'.WS.$this->v('addon').WS.$slug));?>
        </li>
        <?php endforeach;?>
    </ul>
<?php endif;?>