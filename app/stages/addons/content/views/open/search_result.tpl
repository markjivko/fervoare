<?php if(count($this->v('data'))>0):foreach ($this->v('data') AS $result):?>
<h2><?php print a($result['title'],$result['href']);?></h2>
<p><?php print $result['abstract'];?></p>
<hr/>
<?php endforeach;else:?>
<?php print t(__FILE__, 69, 'No results found.');?>
<?php endif;?>