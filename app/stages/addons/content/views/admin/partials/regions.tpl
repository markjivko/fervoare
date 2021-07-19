<?php if (count($this->v('tpl'))>0): foreach ($this->v('tpl') AS $t => $r):?>
<section icon="regions.png" name="<?php print $t;?>.tpl">
    <h1><?php print t(__FILE__, 44, 'Edit %1.tpl\'s regions',$t);?></h1>
    <?php if (count($r)>0):?>
    <?php print form('',null,array('class'=>'x'));?>
    <?php foreach ($r AS $region => $data):?>
    <?php print tag('h2',null,t(__FILE__, 45, 'The "%1" region',ucfirst(strtolower($region)))).textarea($data,array($t.'#'.$region,$t.'_'.$region),array('class'=>'edit'));?>
    <?php endforeach;?>
    <?php print submit(t(__FILE__, 13, 'Update')).form(true);?>
    <?php 
        else:
            print t(__FILE__, 46, 'No regions were defined in this template.');
        endif;
    ?>
</section>
<?php endforeach;
    else: 
        print t(__FILE__, 47, 'No template files found.');
    endif;
?>