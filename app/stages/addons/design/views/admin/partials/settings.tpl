<h1><?php print t(__FILE__, 2, 'Theme "%1" settings',$this->v('theme'));?></h1>
<?php 
    if (count($this->v('data'))>0)
    {
        print form(null,null,array('class'=>'x'));
        foreach ($this->v('data') AS $key => $value)
        {
            $keyName = ucfirst(strtolower(str_replace('_', ' ', $key)));
            if (!is_array($value))
            {
                print label($keyName,$key).input('text',$key,$value).tag('br',null,null,true);
            }
            else
            {
                print tag('h2',null,$keyName.' '.t(__FILE__, 3, '(comma-separated values)')).textarea(implode(',', $value),array($key));
            }
        }
        print submit(t(__FILE__, 4, 'Update')).form('true');
    }
    else
    {
        print t(__FILE__, 5, 'You did not specify any settings in your theme\'s info.ini file.');
    }
?>