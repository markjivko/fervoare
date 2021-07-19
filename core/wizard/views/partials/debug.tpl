<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}?>
<?php if (!is_null($info->errorPage)):?>
    <div class="error_title error"></div>
    <div class="carved">
        This page returned the following HTTP/1.0 Status:<br/>
        <b class=code><?php print $info->errorPage;?></b>
    </div>
    <br/>
<?php endif;?>
<?php if (!is_null($info->redirPage)):?>
    <div class="error_title warning"></div>
    <div class="carved">
        This page redirects to:<br/>
        <b class=code><?php print $info->redirPage;?></b>
    </div>
    <br/>
<?php endif;?>
<div class="slate">
    <table style="width:100%;">
     <tr>
        <th width=50% >Routing Rule</th>
        <th>Routing Path</th>    
      </tr>
    <?php foreach ($info->url_morphs AS $um):?>
      <tr>
        <td><?php echo $um->r[0].' &rArr; '.$um->r[1];?></td>
        <td><?php echo $um->u;?></td>
      </tr>
    <?php endforeach;?>    
    </table>
    <table style="width: 100%;">    
      <tr>
          <th colspan=2>Script information</th>
      </tr>
      <tr>
        <td width=150><i>Stage</i></td>
        <td><?php echo $info->info->stage;?></td>
      </tr>
      <tr>
        <td><i>Controller</i></td>
        <td><?php echo $info->info->controller;?></td>
      </tr>
      <tr>
        <td><i>Method</i></td>
        <td><?php echo $info->info->method;?></td>
      </tr>
      <tr>
        <td><i>Arguments</i></td>
        <td><?php echo json_encode($info->info->parameters);?></td>
      </tr>
      <tr>
          <td><i>Full path</i></td>
          <td><?php echo STAGES . DS . $info->info->stage . DS . 'controllers' . DS . $info->info->controller . EXT;?></td>
      </tr>
    </table>
    <div class="a">
          <span class="tip" title="Click to view/hide the method's source code."><i>Source code</i></span><br/>
          <div class="b">
            <?php 
                print highlight(
                    $info->code,
                    null,
                    array('class'=>'code')
                );
            ?>
        </div>
    </div>
    <table style="width: 100%;">
        <tr>
            <th colspan=2>Available tools</th>
        </tr>
        <tr>
            <td><i>Objects</i></td>
            <td><?php if (count($info->objects)>0):?>
                <?php foreach ($info->objects AS $obj):
                        $obj_name = substr($obj, 0, strrpos($obj, '_'));
                        switch (str_replace($obj_name, '', $obj)){
                            case '_cl': $obj_loc = '-core'; $obj_loc_name='core library'; break;
                            case '_l': $obj_loc = '-app'; $obj_loc_name='application library';break;
                            case '_m': $obj_loc = '-stage='.trim($info->info->stage);$obj_loc_name='stage model';break;
                            default: $obj_loc = ''; $obj_loc_name=''; break;
                        } ?>
                    <a href="search:<?php echo $obj_name;?> <?php echo $obj_loc;?>" title="More information on the <?php echo '['.$obj_name.'] '.$obj_loc_name;?>." class="stop tip"><?php echo $obj_name;?></a>(<?php echo $obj_loc_name?>) 
                <?php endforeach;?>
            <?php endif;?></td>
        </tr>
        <tr>
            <td><i>Helpers</i></td>
            <td><?php if (count($info->helpers)>0):?>
                <?php foreach ($info->helpers AS $obj):
                        $obj_name = substr($obj, 0, strrpos($obj, '_'));
                        switch (str_replace($obj_name, '', $obj)){
                            case '_ch': $obj_loc = '-core'; $obj_loc_name='core helper'; break;
                            case '_h': $obj_loc = '-app'; $obj_loc_name='application helper';break;
                            default: $obj_loc = ''; $obj_loc_name=''; break;
                        } ?>
                    <a href="search:<?php echo $obj_name;?> <?php echo $obj_loc;?>" title="More information on the <?php echo '['.$obj_name.'] '.$obj_loc_name;?>." class="stop tip"><?php echo $obj_name;?></a>(<?php echo $obj_loc_name?>) 
                <?php endforeach;?>
            <?php endif;?></td>
        </tr>
        <tr>
            <td><i>Functions</i></td>
            <td><?php if (count($info->functions)):
                $fnc = array();
                foreach ($info->functions AS $obj):
                    $fnc[] = '<a href="search:'.$obj.'" title="More information on the '.$obj.' function." class="stop tip">'.$obj.'</a>'; 
                endforeach;
                echo implode(', ', $fnc);unset($fnc);
                endif;?>
             </td>
        </tr>
        <tr>
            <th colspan=2>Performance</th>
        </tr>
        <tr>
            <td width=150><i>Execution time</i></td>
            <td><?php echo round($info->exec,9);?> seconds</td>
        </tr>
        <tr>
            <td><i>Memory (peak)</i></td>
            <td><?php echo number_format($info->memory_max/1048576,2);?> MB (<?php echo number_format($info->memory_max,0);?> Bytes)</td>
        </tr>
        <tr>
            <td><i>Memory (average)</i></td>
            <td><?php echo number_format($info->memory/1048576,2);?> MB (<?php echo number_format($info->memory,0);?> Bytes)</td>
        </tr>
        <?php if (!is_null($info->sql)):?>
            <tr>
                <th colspan=2>SQL Statistics</th>
            </tr>
            <tr><td><i>Total queries</i></td><td><?php print $info->sql->n;?></td></tr>
            <tr><td><i>Total execution time</i></td><td><?php print $info->sql->t;?> seconds</td></tr>
            <tr>
                <td colspan=2>
                    <div class="a">
                    <span class="tip" title="Click to view/hide the SQL statements."><i>SQL statements</i></span><br/>
                    <div class="b ob">
                        <?php 
                            print highlight(
                              str_replace(array('<br />',"\n"),"\r\n",print_r(
                                  object_to_array($info->sql->q),
                                  true
                              )),
                              null,
                              array('class'=>'code')
                          );
                        ?>
                    </div>
                  </div>
                </td>
            </tr>
        <?php endif;?>
        <tr>
            <th colspan=2>Variables</th>
        </tr>
        <?php if ($info->session):?>
            <tr>
                <td colspan=2>
                    <div class="a">
                        <span class="tip" title="Click to view/hide the SESSION data."><i>SESSION</i></span><br/>
                        <div class="b ob">
                            <?php 
                                print highlight(
                                    str_replace(
                                        array('<br />',"\n"),
                                        "\r\n",
                                        print_r(
                                            object_to_array($info->session),
                                            true
                                        )
                                    ),
                                    null,
                                    array('class'=>'code')
                                );
                            ?>
                        </div>
                    </div>
                </td>
            </tr>
        <?php endif;?>
        <tr>
            <td colspan=2>
                <div class="a">
                    <span class="tip" title="Click to view/hide the GET data."><i>GET</i></span><br/>
                    <div class="b ob">
                        <?php 
                            print highlight(
                                str_replace(array('<br />',"\n"),"\r\n",print_r(
                                    object_to_array($info->get),
                                    true
                                )),
                                null,
                                array('class'=>'code')
                            );
                        ?>
                    </div>
                </div>
            </td>
        </tr>
        <tr>
            <td colspan=2>
                <div class="a">
                    <span class="tip" title="Click to view/hide the POST data."><i>POST</i></span><br/>
                    <div class="b ob">
                        <?php 
                            print highlight(
                                str_replace(
                                    array('<br />',"\n"),
                                    "\r\n",
                                    print_r(
                                        object_to_array($info->post),
                                        true
                                    )
                                ),
                                null,
                                array('class'=>'code')
                            );
                        ?>
                    </div>
                </div>
            </td>
        </tr>
        <?php if ($info->flags):?>
            <tr>
                <td colspan=2>
                    <div class="a">
                        <span class="tip" title="Click to view/hide the OctoMS Flags."><i>FLAGS</i></span><br/>
                        <div class="b ob">
                            <?php 
                                print highlight(
                                    str_replace(
                                        array('<br />',"\n"),
                                        "\r\n",
                                        print_r(
                                            object_to_array($info->flags),
                                            true
                                        )
                                    ),
                                    null,
                                    array('class'=>'code')
                                );
                            ?>
                        </div>
                    </div>
                </td>
            </tr>
        <?php endif;?>
        <tr>
            <th colspan=2>Output</th>
        </tr>
        <tr>
            <td colspan=2>
                <div class="a">
                    <span class="tip" title="Click to view/hide the sent headers."><i>Headers</i></span><br/>
                    <div class="b ob adjust" style="overflow-x:scroll;overflow-y:hidden;">
                        <?php echo str_replace(array("\n","\t"), array('<br/>','&nbsp;'), htmlentities($info->headers));?>
                    </div>
                </div>
                <?php if (!is_null($info->buffer) && strlen($info->buffer)>0): ?>
                    <div class="a">
                        <span class="tip" title="Click to view/hide the output buffer."><i>Content</i></span><br/>
                        <div class="b ob">
                          <?php echo str_replace(array("\n","\t"), array('<br/>','&nbsp;'), htmlentities($info->buffer, ENT_QUOTES, "UTF-8"));?>
                        </div>
                    </div>
                <?php endif;?>
            </td>
        </tr>
    </table>
</div>