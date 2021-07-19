<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}?>
<?php 
    if (count($this->v('info'))>0):
        foreach ($this->v('info') AS $result):
            if (count($result)>3):
                $params = array(); 
                foreach ($result AS $k => $v):
                    if (strpos($k, 'param')!==false):
                        $params[] = $v;
                    endif;
                endforeach;
?>
    <div>
        <table class="search-table">
        <tr><th class="method"><?php echo $result['res']['method'];?></th></tr>
        <tr>
            <td><div class="div-h2">Description</div></td>
        </tr>
        <tr>
            <td><?php if (isset($result['description'])): echo $result['description']; endif;?></td>
        </tr>
        <tr>
            <td class="rarr">
                <table>
                    <tr>
                        <th style="width: 50%;">
                            Parameters
                        </th>
                        <th style="width: 50%;">
                            Return
                        </th>
                    </tr>
                    <tr>
                        <td class="params tip" title="Parameters">
                            <?php if (count($params)>0):?>
                                <ol>
                                    <?php foreach ($params AS $p):?>
                                        <li><?php echo $p;?></li>
                                    <?php endforeach;?>
                                </ol>
                            <?php endif;?>
                        </td>
                        <td class="return tip" title="Return">
                            <div>
                                <?php if (isset($result['return'])): echo $result['return']; else: echo '(void)';endif;?>
                            </div>
                        </td>
                    </tr>
                </table>
            </td>
        </tr>
        <tr>
            <td><div class="div-h2">Examples</div></td>
        </tr>
        <tr>
            <td>
                <?php if (isset($result['deploy'])):?>
                <div class="a">
                    <span class="tip" title="Click to view/hide information on how to gain access to this method.">How to deploy.</span><br/>
                    <div class="b">
                        <table class="code">
                            <?php foreach ($result['deploy'] AS $line):?>
                                <tr><td><?php echo $line;?></td></tr>
                            <?php endforeach;?>
                        </table>
                    </div>
                </div>
                <?php endif;?>
                <?php if (isset($result['example'])):?>
                <table class="code">
                    <?php foreach ($result['example'] AS $line):?>
                        <tr><td><?php echo $line;?></td></tr>
                    <?php endforeach;?>
                </table>
                <?php else:?>
                No examples provided.
                <?php endif;?>
            </td>
        </tr>
        <tr>
            <td>
                <span style="font-size: 10px;padding: 0 5px;">
                <?php echo $result['file'];?>
                <?php if (isset($result['author'])||isset($result['link'])):?>
                    | 
                    <?php if (isset($result['link'])):?><a href="<?php echo str_replace("\n",'',$result['link']);?>" class="tip" title="Click to visit <?php echo str_replace("\n", '', $result['link']);?>" target="_blank"><?php endif;?>
                    <?php if (isset($result['author'])):?>Created by <i><?php echo trim($result['author']);?></i><?php else:?>More info<?php endif;?>
                    <?php if (isset($result['link'])):?></a><?php endif;?>
                <?php endif;?>
                </span>
            </td>
        </tr>
        </table>
    </div>
<?php endif;endforeach;else:?>
    <div>No results to display.</div>
<?php endif;?>