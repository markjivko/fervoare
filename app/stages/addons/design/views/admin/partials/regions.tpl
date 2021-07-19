<?php if (count($this->v('tpl'))>0): foreach ($this->v('tpl') AS $t => $r):?>
<section icon="regions.png" name="<?php print $t;?>.tpl">
    <h1>Edit <?php print $t;?>.tpl's regions</h1>
    <?php if (count($r)>0):?>
    <?php print form('',null,array('class'=>'x'));?>
    <?php foreach ($r AS $region => $data):?>
    <?php print tag('h2',null,'The "'.ucfirst(strtolower($region)).'" region').textarea($data,array('name'=>$t.'#'.$region,'id'=>$t.'_'.$region,'class'=>'edit'));?>
    <?php endforeach;?>
    <?php print submit('Update').form(true);?>
    <?php else:?>
    No regions were defined in this template.
    <?php endif;?>
</section>
<?php endforeach; else: ?>
    No template files found.
<?php endif;?>