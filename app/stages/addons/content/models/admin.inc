<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko <stephino.team@gmail.com>
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
/*
 * Admin model
 */
class admin_m extends octoms {
    /*
     * Set the pages upload directory
     */
    public $uploadDir = 'pages';

    /**
     * Get the regions in the provided template
     */
    public function region_get($tpl = '') {
        // Get the regions from the template file
        if (file_exists($f = APP_TPL . DS . (is_null($gTpl = $this->settings->get('general.template')) ? 'default' : $gTpl) . DS . 'open' . DS . $tpl . '.tpl')) {
            preg_match_all('/\$this[\W]*->[\W]*region[^(]*\(([^)]*)\)/i', file_get_contents($f), $m);
            if (isset($m[1])) {
                // Get the settings from the database
                $result = array();
                foreach ($m[1] as $region) {
                    if (false !== $pos = strpos($region, ','))
                        $region = substr($region, 0, $pos);
                    $region = str_replace(array('"', '\''), '', $region);
                    $result[$region] = $this->settings->get(
                        sprintf(
                            'regions.%s.%s',
                            $tpl,
                            $region
                        )
                    );
                }

                // Return the results
                return $result;
            }
        }

        // Remove unused regions from the database
        $setRegions = array_keys(
            $this->settings->getChildren(
                sprintf(
                    'regions.%s',
                    $tpl
                )
            )
        );
        
        $avaRegions = array_keys($result);
        foreach ($setRegions as $r) {
            if (!in_array($r, $avaRegions)) {
                // Remove the key
                $this->settings->del(
                    sprintf(
                        'regions.%s.%s',
                        $tpl,
                        $r
                    )
                );
            }
        }

        return null;
    }// end function region_get()

    /**
     * List all available template files
     */
    public function region_list() {
        $r = array();
        if (is_dir($f = APP_TPL . DS . (is_null($gTpl = $this->settings->get('general.template')) ? 'default' : $gTpl) . DS . 'open')) {
            if (false !== $h = opendir($f)) {
                while (false !== ($file = readdir($h))) {
                    if (is_file($f . DS . $file) && substr($file, strrpos($file, '.')) == '.tpl') {
                        $r[] = substr($file, 0, strrpos($file, '.'));
                    }
                }
            }
        }
        return $r;
    }// end function region_list()

    /**
     * Set the region for the provided template
     */
    public function region_set($tpl = '', $region = '', $data = '') {
        try {
            // Verify the region exists in the given template
            if (!in_array($region, array_keys($regions = (array) $this->region_get($tpl)))) {
                $this->template->api(
                        t(__FILE__, 19, 'Invalid region "%1" in template "%2".', $region, $tpl),
                        false
                );
                return;
            }

            // Remove all the useless settings
            if (count($settings = (array) $this->settings->get(
                sprintf(
                    'regions.%s',
                    $tpl
                )
            )) > 0) {
                foreach ($settings as $s => $v) {
                    if (!isset($regions[$s])) {
                        $this->settings->del(
                            sprintf(
                                'regions.%s.%s',
                                $tpl,
                                $s
                            )
                        );
                    }
                }
            }

            // Set the data to the DataBase
            $this->settings->set(
                sprintf(
                    'regions.%s.%s',
                    $tpl,
                    $region
                ),
                $data
            );

            // Set a success message
            $this->template->api(
                t(__FILE__, 20, 'The "%1" region in template "%2" was updated.', $region, $tpl)
            );
        } catch (Exception $e) {
            $this->template->api(
                t(__FILE__, 21, 'Error: %1', $e),
                false
            );
        }
    }// end function region_set()

    /**
     * Update regions
     */
    public function region_update() {
        foreach ((array) $this->input->sg('post') as $region => $data) {
            if ($region != '#*#') {
                list($tpl, $r) = explode('#', $region);
                $this->region_set($tpl, $r, $data);
            }
        }
        $this->template->api(
            t(__FILE__, 22, 'Regions successfully updated for template file "%1.tpl".', $tpl)
        );
    }// end function region_update()

    /**
     * Add a post
     */
    public function posts_add() {
        try {
            // Get an empty post
            $this->sql->query(
                $this->sql->select(
                    null,
                    'posts',
                    $this->sql->where(
                        array(
                            'content' => '',
                            'lang' => lang()
                        )
                    )
                )
            );

            // Found an empty post
            if ($this->sql->num_rows() > 0) {
                // Get the first element
                $row = $this->sql->first();

                // Return the id
                return $row['id'];
            }
            // Not found; insert one
            else {
                $this->sql->query(
                    $this->sql->insert(
                        'posts',
                        array(
                            'title' => '',
                            'slug' => '',
                            'type' => 1,
                            'status' => 0,
                            'categ' => 0,
                            'content' => '',
                            'options' => '',
                            'modified' => time(),
                            'created' => time(),
                            'lang' => lang(),
                            'author_id' => $this->auth->user(true)->id
                        )
                    )
                );

                
                // Return the posts id
                return $this->sql->insert_id();
            }
        } catch (Exception $exc) {
            octoms('error', OMS_CL)->log($exc->getMessage());
            return false;
        }
    }// end function posts_add()

    /**
     * Edit page
     */
    public function posts_read($args = array()) {
        if (!isset($args[0])) {
            return array('error' => t(__FILE__, 27, 'Invalid id'));
        }
        $id = $args[0];
        try {
            // Get the data
            $data = $this->sql->query(
                $this->sql->select(
                    null,
                    'posts',
                    $this->sql->where(
                        array(
                            'id' => $args[0],
                            'lang' => lang()
                        )
                    )
                )
            )->first();
            if ($data != false) {
                // Decompress the content and prepare it for display
                $data['content'] = $this->input->clean_html($this->sql->decompress($data['content']));

                // Unserialize the options
                $data['options'] = $this->sql->unserialize($data['options']);

                // Return the data
                return array('data' => $data);
            } else {
                $this->template->redirect('admin/content/posts');
            }
        } catch (Exception $e) {
            return array('error' => $e->getMessage());
        }
    }// end function posts_read()

    /**
     * List all posts and pages
     */
    public function posts_list() {
        // Select all posts
        $this->sql->query(
            $this->sql->select(
                null,
                'posts',
                $this->sql->where(
                    array(
                        'lang' => lang()
                    )
                )
            )
        );

        // Found some posts
        if ($this->sql->num_rows() > 0) {
            // Decompress and unserialize where necessary
            foreach ($rows = $this->sql->rows() as $k => $v) {
                $rows[$k]['content'] = $this->sql->decompress($v['content']);
                $rows[$k]['options'] = $this->sql->unserialize($v['options']);
            }
            return $rows;
        } else {
            return array();
        }
    }// end function posts_list()

    /**
     * Update page
     */
    public function posts_update($args = null) {
        try {
            if (isset($args[1])) {
                // JS or CSS section update
                if (in_array($args[1], array('js', 'css', 'seo'))) {
                    // Get the options
                    $data = $this->sql->query(
                        $q = $this->sql->select(
                            null,
                            'posts',
                            $this->sql->where(
                                array(
                                    'id' => $args[0],
                                    'lang' => lang()
                                )
                            )
                        )
                    )->first();

                    if ($data !== false) {
                        // Get the options
                        $options = $this->sql->unserialize($data['options']);
                        if (is_null($options))
                            $options = array();

                        // Update the options
                        $options[$args[1]] = in_array($args[1], array('js', 'css')) ?
                            array(
                                'server' => explode("\n", $this->input->post('server', 'all')),
                                'onpage' => $this->input->post('onpage')
                            ) :
                            array(
                                'keywords' => explode(',', $this->input->post('keywords', 'all')),
                                'description' => $this->input->post('description', 'all')
                            );

                        // Update the DB
                        $this->sql->query(
                            $this->sql->update(
                                'posts',
                                array(
                                    'options' => $this->sql->serialize($options)
                                ),
                                $this->sql->where(
                                    array(
                                        'id' => $args[0],
                                        'lang' => lang()
                                    )
                                )
                            )
                        );

                        // All done.
                        in_array($args[1], array('js', 'css')) ?
                            $this->template->api(
                                t(__FILE__, 28, 'The %1 scripts were updated for page #%2.', ucfirst(strtolower($args[1])), $args[0])
                            ) :
                            $this->template->api(
                                t(__FILE__, 29, 'The keywords and description were updated for page #%1.', $args[0])
                        );
                    } else {
                        $this->template->api(
                            t(__FILE__, 30, 'Update failed. Reload the page and try again.'),
                            false
                        )->api_cnt($q);
                    }
                }
            }
            // Content update
            else {
                // Update the content
                if (isset($args[0]) && !empty($args[0]) && !is_null($this->input->post('title'))) {
                    // Format the title
                    $title = $this->input->post('title', 'all');

                    // Format the slug
                    $slug = $this->input->post('slug', 'all');
                    $slug = preg_replace(
                        array('%[^0-9a-zA-Z\-\+\_ ]?%', '%[ ]+%'),
                        array('', '-'),
                        strtolower($slug ? $slug : $title)
                    );

                    // Get the type
                    $type = $this->input->post('type', 'all');

                    // If this is an article page, verify it is different from the blog
                    if ($type == 0 && ($slug == $this->settings->get('seo.blog_slug') || $slug == 'blog')) {
                        $slug = '_' . $slug;
                    }

                    // Get the status
                    $status = $this->input->post('status', 'all');

                    // Format the content
                    $content = $this->input->tags_close(
                        $this->input->tags_whipe(
                            $this->input->post(
                                'content',
                                'whitespaces'
                            )
                        )
                    );

                    // Get the language equal
                    $eq_to = $this->input->post('eq_to', 'all');
                    $eq_to = (!is_null($eq_to_free = $this->posts_eq_to($eq_to)) && in_array($eq_to, array_keys($eq_to_free))) 
                        ? intval($eq_to)
                        : 0;
                    
                    $query = $this->sql->update(
                        'posts',
                        array(
                            'title'   => $title,
                            'slug'    => $slug,
                            'type'    => $type,
                            'status'  => $status,
                            'eq_to'   => $eq_to,
                            'content' => $this->sql->compress(
                                $content
                            ),
                            'modified' => time()
                        ),
                        $this->sql->where(
                            array(
                                'id'   => $args[0],
                                'lang' => lang()
                            )
                        )
                    );
                    
                    // Update the posts
                    $this->sql->query($query);

                    // Update the menu
                    if (false !== $menus = $this->_menu_read_all()) {
                        foreach ($menus as $menu) {
                            // Static page. Update entry in menu
                            if ($type == '0') {
                                $this->_menu_item_update($menu, 'post' . $args[0], i18n_baseurl($slug));
                            }
                            // Blog post. Delete entry from menu
                            else {
                                $this->_menu_item_delete($menu, 'post' . $args[0]);
                            }
                        }
                    }

                    // Notify the user
                    $this->template
                        ->api(t(__FILE__, 31, 'The content of page #%1 was updated.', $args[0]))
                        ->api(array('post_update'))
                        ->api_cnt(
                            array(
                                'title' => $title,
                                'slug' => $slug,
                                'status' => $status,
                                'eq_to' => $eq_to,
                                'content' => $content
                            )
                        );
                }
            }
        } catch (Exception $exc) {
            $this->template->api(
                t(__FILE__, 39, 'Update failed. Make sure there is no other post with the type and slug you provided.'),
                false
            )->api_cnt($exc->getMessage());
        }
    }// end function posts_update()

    /**
     * Delete a post
     */
    public function posts_delete($id) {
        try {
            // Delete the page
            $this->sql->query(
                $this->sql->delete(
                    'posts',
                    $this->sql->where(
                        array(
                            'id' => $id,
                            'lang' => lang()
                        )
                    )
                )
            );

            // Delete post from the menu
            if (false !== $menus = $this->_menu_read_all()) {
                foreach ($menus as $menu) {
                    $this->_menu_item_delete($menu, 'post' . $id);
                }
            }

            // Delete references to this page
            $this->sql->query(
                $this->sql->update(
                    'posts',
                    array(
                        'eq_to' => 0
                    ),
                    $this->sql->where(
                        array('eq_to' => $id)
                    )
                )
            );

            // Notify the user
            $this->template
                ->api(t(__FILE__, 32, 'The page #%1 was deleted.', $id))
                ->api(array('delRow'));
        } catch (Exception $e) {
            $this->template->api(
                t(__FILE__, 33, 'The delete action failed.'),
                false
            );
        }
    }// end function posts_delete()

    /**
     * Posts - i18n equivalent
     */
    public function posts_eq_to($eq_to = null, $pattern = null) {
        if (is_null($pattern))
            $pattern = '%s - %s';
        try {
            if ($this->sql->run('eq_to_free', $eq_to)->num_rows() > 0) {
                $result = array('0' => '');
                $this->settings->setLang('en');
                foreach ($this->sql->rows() as $row) {
                    $result[strval($row['id'])] = sprintf(
                        $pattern,
                        $row['title'],
                        $row['type'] == 1 ?
                        baseurl($this->settings->get('seo.blog_slug') . WS . $row['slug']) :
                        baseurl($row['slug'])
                    );
                }
                $this->settings->setLang();
                return $result;
            } else {
                return null;
            }
        } catch (Exception $e) {
            return null;
        }
    }// end function posts_eq_to()

    /**
     * Add a link
     */
    public function links_add() {
        try {
            // Get an empty link row
            $row = $this->sql->query(
                $this->sql->select(
                    null,
                    'links',
                    $this->sql->where(
                        array(
                            'title' => '',
                            'href' => '',
                            'lang' => lang()
                        )
                    )
                )
            )->first();

            // Return the link page
            return is_numeric($row['id']) ?
                $row['id'] :
                $this->sql->query(
                    $this->sql->insert(
                        'links',
                        array(
                            'title' => '',
                            'href' => '',
                            'lang' => lang()
                        )
                    )
                )->insert_id();
        } catch (Exception $e) {
            return $e->getMessage();
        }
    }// end function links_add()

    /**
     * Read a link
     */
    public function links_read($id = null) {
        if (is_null($id))
            return array('error' => t(__FILE__, 27, 'Invalid id'));
        try {
            // Get the data
            $data = $this->sql->query(
                    $this->sql->select(
                        null,
                        'links',
                        $this->sql->where(
                            array(
                                'id' => $id,
                                'lang' => lang()
                            )
                        )
                    )
                )->first();

            if ($data != false) {
                // Return the data
                return array('data' => $data);
            } else {
                $this->template->redirect('admin/content/links');
            }
        } catch (Exception $e) {
            return array('error' => $e->getMessage());
        }
    }// end function links_read()

    /**
     * List all links
     */
    public function links_list() {
        $rows = array();

        if ($this->sql->query(
                $this->sql->select(
                    null,
                    'links',
                    $this->sql->where(
                        array(
                            'lang' => lang()
                        )
                    )
                )
            )->num_rows() > 0) {
            $rows = $this->sql->rows();
        }

        return $rows;
    }// end function links_list()

    /**
     * Update a link
     */
    public function links_update($args = null) {
        try {
            if (isset($args[0]) && !empty($args[0])) {
                $id = $args[0];
                $title = $this->input->post('title', 'all');
                $href = $this->input->post('href', 'all');

                // Perform the update
                $this->sql->query(
                    $this->sql->update(
                        'links',
                        array(
                            'title' => $title,
                            'href' => $href
                        ),
                        $this->sql->where(
                            array(
                                'id' => $id,
                                'lang' => lang()
                            )
                        )
                    )
                );

                // Update menu item
                if (false !== $menus = $this->_menu_read_all()) {
                    foreach ($menus as $menu) {
                        $this->_menu_item_update($menu, 'link' . $id, $href);
                    }
                }

                // Set the api response
                $this->template->api(
                    t(__FILE__, 34, 'The link #%1 was updated.', $args[0])
                );
            } else {
                $this->template->api(
                    t(__FILE__, 35, 'The link could not be updated because it lacks an ID.'),
                    false
                );
            }
        } catch (Exception $e) {
            $this->template->api(
                t(__FILE__, 30, 'Update failed. Reload the page and try again.')
            )->api_cnt($e->getMessage());
        }
    }// end function links_update()

    /**
     * Delete a link
     */
    public function links_delete($id) {
        try {
            // Delete the link
            $this->sql->query(
                $this->sql->delete(
                    'links',
                    $this->sql->where(
                        array(
                            'id' => $id,
                            'lang' => lang()
                        )
                    )
                )
            );

            // Delete link from the menu
            if (false !== $menus = $this->_menu_read_all()) {
                foreach ($menus as $menu) {
                    $this->_menu_item_delete($menu, 'link' . $id);
                }
            }

            $this->template
                ->api(
                        t(__FILE__, 36, 'The link #%1 was deleted.', $id)
                )->api(array('delRow'));
        } catch (Exception $e) {
            $this->template->api(
                t(__FILE__, 33, 'The delete action failed.'),
                false
            );
        }
    }// end function links_delete()

    /**
     * Read the menu
     */
    public function menu_read() {
        // Define the list pattern
        // ID, type (post,link), title, url
        $pattern = '[x_%s]<div class="handle"></div><p><span class="icon %s"></span>%s<span class="small">%s</span></p>';
        $menu_pattern = '[x_%s]<div class="handle"></div><p><span class="icon %s"></span><span class="editable">%s</span><span class="small">%s</span></p>';

        // Get the static pages
        $posts = array();
        $this->sql->query(
            $this->sql->select(
                null,
                'posts',
                $this->sql->where(
                    array(
                        'lang' => lang(),
                        'type' => 0
                    )
                )
            )
        );
        if ($this->sql->num_rows() > 0) {
            foreach ($this->sql->rows() as $row) {
                $posts[] = sprintf(
                    $pattern,
                    'post' . $row['id'],
                    'post',
                    $row['title'],
                    $row['type'] == 1 ?
                    i18n_baseurl($this->settings->get('seo.blog_slug') . WS . $row['slug']) :
                    i18n_baseurl($row['slug'])
                );
            }
        }
        $posts = l(
            $posts,
            true,
            array(
                'class' => 'posts con',
                'prepend' => tag(
                    'span',
                    null,
                    t(__FILE__, 1, 'Static pages')
                )
            )
        );

        // Get the links
        $links = array();
        $this->sql->query(
            $this->sql->select(
                null,
                'links',
                $this->sql->where(
                    array(
                        'lang' => lang()
                    )
                )
            )
        );
        if ($this->sql->num_rows() > 0) {
            foreach ($this->sql->rows() as $row) {
                $links[] = sprintf(
                    $pattern,
                    'link' . $row['id'],
                    'link',
                    $row['title'],
                    $row['href']
                );
            }
        }
        $links = l(
            $links,
            true,
            array(
                'class' => 'links con',
                'prepend' => tag(
                    'span',
                    null,
                    t(__FILE__, 2, 'Links')
                )
            )
        );

        // Get the menus
        $menus = '';
        $this->sql->query(
            $this->sql->select(
                null,
                'menus',
                $this->sql->where(
                    array(
                        'lang' => lang()
                    )
                )
            )
        );
        if ($this->sql->num_rows() > 0) {
            foreach ($this->sql->rows() as $row) {
                $menus .= l(
                    (false !== $_l = $this->_menu_parse_to_list(
                        $this->sql->unserialize($row['content']),
                        $menu_pattern
                    )) 
                        ? $_l 
                        : array(),
                    true,
                    array(
                        'class' => 'menu con',
                        'rel' => $row['name'],
                        'prepend' => tag('span', array('class' => 'editable name'), $row['name']) . tag('span', array('class' => 'delete', 'title' => t(__FILE__, 37, 'Delete menu'), 'confirm' => t(__FILE__, 38, 'Are you sure you want to delete this menu?')))
                    )
                );
            }

            // Add an empty container
            if (strlen($menus) == 0) {
                $menus = tag(
                    'ol',
                    array(
                        'class' => 'menu con',
                        'rel' => 'default',
                        'prepend' => tag('span', array('class' => 'editable'), 'default') . tag('span', array('class' => 'delete'))
                    )
                );
            }
        }

        // Return the result
        return array(
            'm' => $menus, 'p' => $posts, 'l' => $links
        );
    }// end function menu_read()

    /**
     * Update the menu
     */
    public function menu_update() {
        if ($this->input->post()) {
            // Add a new menu
            if ($this->input->post('*add')) {
                if (false !== $this->_menu_read('new_menu', true)) {
                    $this->template->api(
                        t(__FILE__, 3, 'You cannot add another "new_menu".'),
                        false
                    );
                } else {
                    // Add the menu
                    if (false == $this->_menu_update('new_menu')) {
                        $this->template->api(
                            t(__FILE__, 4, 'Could not add "new_menu".'),
                            false
                        );
                    } else {
                        $this->template
                            ->api(
                                t(__FILE__, 5, 'New menu added successfully.')
                            )
                            ->api(array('menu_refresh'))
                            ->api_cnt($this->menu_read());
                    }
                }
                return;
            }

            // Process each individual menu
            foreach ($_POST as $menu => $command) {
                // Exclude the Fervoare API triggering key
                if ($menu != '#*#') {
                    if (is_string($command)) {
                        parse_str($command, $order);
                    } else {
                        $order = $command;
                    }

                    // X is the key
                    if (is_array($order) && array_key_exists('x', $order) && is_array($order['x'])) {
                        // Get the old items and order
                        if (false !== $old_menu = $this->_menu_read($menu)) {
                            $old_items = $old_menu['items'];
                        }

                        // Prepare the new order
                        $new_items = array();
                        $new_order = array();
                        foreach ($order['x'] as $key => $value) {
                            // A new element
                            if (!is_numeric($key)) {
                                // Get the new item
                                if (false === $item = $this->_menu_item_get($key)) {
                                    // Continue if the item does not exist
                                    continue;
                                }
                            } else {
                                $item = isset($old_items[$key]) ? $old_items[$key] : null;
                                $key = intval($key);
                            }

                            // Create the new order
                            if (!is_null($item)) {
                                $new_order[$key] = $value !== 'root' ? intval($value) : 'root';
                                $new_items[$key] = $item;
                            }
                        }

                        // Numeric keys only
                        $new_items_keys = array_keys($new_items);
                        $new_items = array_values($new_items);
                        $new_order_parsed = array();
                        foreach ($new_order as $_k => $_v) {
                            $new_order_parsed[
                                    array_search(
                                            $_k,
                                            $new_items_keys,
                                            true
                                    )
                                    ] = (
                                    $_v !== 'root' ?
                                    array_search(
                                            $_v,
                                            $new_items_keys,
                                            true
                                    ) :
                                    'root'
                                    );
                        }
                        $new_order = $new_order_parsed;

                        // Save to the database
                        try {
                            $this->_menu_update(
                                    $menu,
                                    array(
                                        'items' => $new_items,
                                        'order' => $new_order
                                    )
                            );
                        } catch (Exception $e) {
                            // Stop the procedures
                            $this->template->api(
                                    t(__FILE__, 6, 'Menu update failed. Please try again.'),
                                    false
                            );
                            return false;
                        }
                    }
                    // Change the menu's name
                    elseif (is_array($order) && isset($order['new_name'])) {
                        // Sanitize input
                        $menu = $this->input->clean_all($menu);
                        $name = $this->input->clean_all(urldecode($order['new_name']));
                        $name = strtolower(preg_replace('/[^a-zA-Z0-9\_]/i', '_', $name));

                        // Does the menu exist?
                        if ($this->_menu_read($menu, true)) {
                            if ($menu != $name) {
                                // Run the SQL
                                try {
                                    // Update the name (if it is different)
                                    $this->sql->query(
                                        $this->sql->update(
                                            'menus',
                                            array('name' => $name),
                                            $this->sql->where(
                                                array(
                                                    'name' => $menu,
                                                    'lang' => lang()
                                                )
                                            )
                                        )
                                    );

                                    // Notify the user
                                    $this->template
                                        ->api(
                                            t(__FILE__, 7, 'Menu "%1" changed to "%2".', $menu, $name)
                                        )->api(array('menu_update_name'))
                                        ->api_cnt(
                                            array(
                                                'menu' => $menu,
                                                'name' => $name,
                                            )
                                        );
                                } catch (Exception $e) {
                                    $this->template->api(
                                        t(__FILE__, 8, 'Menu name change failed. Please refresh and try again.'),
                                        false
                                    );
                                }
                            } else {
                                // No changes made
                                $this->template
                                    ->api(
                                        t(__FILE__, 9, 'No changes were made to menu "%1".', $menu),
                                        'info'
                                    )
                                    ->api(array('menu_update_name'))
                                    ->api_cnt(
                                        array(
                                            'menu' => $menu,
                                            'name' => $name
                                        )
                                    );
                            }
                        } else {
                            $this->template->api(
                                t(__FILE__, 10, 'Menu "%1" does not exist.', $menu),
                                false
                            );
                        }

                        // One at a time
                        return;
                    }
                    // Update an item in the menu
                    elseif (is_array($order) && isset($order['item_id']) && isset($order['item_name'])) {
                        // Sanitize the item
                        $menu = $this->input->clean_all($menu);
                        $id = $this->input->clean_all(urldecode($order['item_id']));
                        $name = $this->input->clean_all(urldecode($order['item_name']));
                        $name = preg_replace('/\r|\n|\s{2,}/i', ' ', $name);

                        // Change item name
                        if (false === $content = $this->_menu_read($menu)) {
                            $this->template->api(
                                t(__FILE__, 11, 'Menu "%1" does not exist. Please refresh and try again.', $menu),
                                false
                            );
                        } else {
                            // The Items id
                            $_id = str_replace('x_', '', $id);

                            // Modify the item
                            if (!isset($content['items'][$_id])) {
                                $this->template->api(
                                    t(__FILE__, 12, 'Invalid item id. Please refresh and try again.'),
                                    false
                                );
                                return;
                            } elseif ($content['items'][$_id][1] == $name) {
                                $this->template->api(
                                    t(__FILE__, 13, 'No changes were made to item "%1" from menu "%2".', $name, $menu),
                                    'info'
                                );
                                return;
                            }
                            $content['items'][$_id][1] = $name;

                            // Update the menu
                            if (false === $this->_menu_update($menu, $content)) {
                                // Something went wrong
                                $this->template->api(
                                    t(__FILE__, 14, 'Could not rename the item. Please refresh and try again.'),
                                    false
                                );
                            } else {
                                // Notify the user
                                $this->template
                                    ->api(
                                        t(__FILE__, 15, 'Item name changed to "%1" in menu "%2".', $name, $menu)
                                    )
                                    ->api(array('menu_update_item'))
                                    ->api_cnt(
                                        array(
                                            'menu' => $menu,
                                            'item_id' => $id,
                                            'item_name' => $name
                                        )
                                    );
                            }
                        }

                        // One at a time
                        return;
                    }
                    // Delete an entire menu
                    elseif (is_array($order) && isset($order['delete'])) {
                        // Remove the menu
                        if (false !== $this->_menu_read($menu, true)) {
                            try {
                                $this->sql->query(
                                    $this->sql->delete(
                                        'menus',
                                        $this->sql->where(
                                            array(
                                                'name' => $menu,
                                                'lang' => lang()
                                            )
                                        )
                                    )
                                );
                            } catch (Exception $e) {
                                $this->template->api(
                                    t(__FILE__, 16, 'Menu "%1" could not be erased. Please refresh and try again.', $menu),
                                    false
                                );
                                return;
                            }
                        } else {
                            $this->template->api(
                                t(__FILE__, 16, 'Menu "%1" could not be erased. Please refresh and try again.', $menu),
                                false
                            );
                            return;
                        }

                        // Notify the user
                        $this->template
                                ->api(
                                    t(__FILE__, 17, 'Menu "%1" removed successfully.', $menu)
                                )
                                ->api(array('menu_refresh'))
                                ->api_cnt($this->menu_read());

                        // One at a time
                        return;
                    } else {
                        // Empty the menu
                        $this->_menu_update($menu, null);
                    }
                }
            }

            // Update successful
            $this->template
                ->api(t(__FILE__, 18, 'Menus updated successfully.'))
                ->api(array('menu_refresh'))
                ->api_cnt($this->menu_read());
        }
    }// end function menu_update()

    /**
     * Menu helper - Read the structure
     */
    protected function _menu_read($menuName = null, $fastCheck = false) {
        try {
            // Get the menu
            $this->sql->query(
                $this->sql->select(
                    null,
                    'menus',
                    $this->sql->where(
                        array(
                            'name' => $menuName,
                            'lang' => lang()
                        )
                    )
                )
            );

            // Found somethings?
            if ($this->sql->num_rows() > 0) {
                // Use this to verify the menu exists
                if ($fastCheck)
                    return true;

                // Get the first result
                $row = $this->sql->first();

                // Parse the content
                return $this->sql->unserialize($row['content']);
            } else {
                return false;
            }
        } catch (Exception $e) {
            return false;
        }
    }// end function _menu_read()

    /**
     * Menu Helper - get all menus in the current language
     */
    protected function _menu_read_all() {
        try {
            // Get the menu
            $this->sql->query(
                $this->sql->select(
                    array('name'),
                    'menus',
                    $this->sql->where(
                        array(
                            'lang' => lang()
                        )
                    )
                )
            );

            // Found somethings?
            if ($this->sql->num_rows() > 0) {
                $result = array();
                foreach ($this->sql->rows() as $row) {
                    $result[] = $row['name'];
                }
                return $result;
            } else {
                return false;
            }
        } catch (Exception $e) {
            return false;
        }
    }// end function _menu_read_all()

    /**
     * Menu helper - delete in item from the menu
     */
    protected function _menu_item_delete($menu, $tag) {
        // Get the content
        if (false !== $content = $this->_menu_read($menu)) {
            if (isset($content['items'])) {
                foreach ($content['items'] as $key => $item) {
                    // Found the tag?
                    if ($item[2] === $tag) {
                        // Remove this key
                        unset($content['items'][$key]);

                        // Remove from the items order
                        unset($content['order'][$key]);

                        // Remove all traces to this item from the order
                        foreach ($content['order'] as $k => $order) {
                            if ($order === $key)
                                unset($content['order'][$k]);
                        }
                    }
                }

                // Update the menu
                return $this->_menu_update($menu, $content);
            }
        }

        // Something went wrong
        return false;
    }// end function _menu_item_delete()

    /**
     * Menu helper - get an item from the database
     */
    protected function _menu_item_get($tag) {
        if (preg_match('/([a-zA-Z]*)([0-9]*)/i', $tag, $regs)) {
            $table = $regs[1] . 's';
            $id = $regs[2];

            try {
                if (
                    $this->sql->query(
                        $this->sql->select(
                            null,
                            $table,
                            $this->sql->where(
                                array(
                                    'id' => $id,
                                    'lang' => lang()
                                )
                            )
                        )
                    )->num_rows() > 0
                ) {
                    // Get the SQL result
                    $row = $this->sql->first();

                    // Manage cases
                    switch ($table) {
                        // Posts
                        case 'posts':
                            $result = array(
                                $row['type'] == 1 ?
                                i18n_baseurl($this->settings->get('seo.blog_slug') . WS . $row['slug']) :
                                i18n_baseurl($row['slug']),
                                $row['title'],
                                $tag
                            );
                            break;

                        // Links
                        case 'links':
                            $result = array(
                                $row['href'],
                                $row['title'],
                                $tag
                            );
                            break;

                        // Unkwnown table
                        default: $result = false;
                    }

                    // Return the result
                    return $result;
                } else {
                    return false;
                }
            } catch (Exception $e) {
                return false;
            }
        }
    }// end function _menu_item_get()

    /**
     * Update an item from a menu
     */
    protected function _menu_item_update($menu, $tag, $url, $title = null) {
        // Get the content
        if (false !== $content = $this->_menu_read($menu)) {
            if (isset($content['items'])) {
                foreach ($content['items'] as $key => $item) {
                    // Found the tag?
                    if ($item[2] === $tag) {
                        if (is_null($url))
                            $url = $item[0];
                        if (is_null($title))
                            $title = $item[1];
                        $content['items'][$key] = array(
                            $url,
                            $title,
                            $tag
                        );
                    }
                }

                // Update the menu
                return $this->_menu_update($menu, $content);
            }
        }

        // Something went wrong
        return false;
    }// end function _menu_item_update()

    /**
     * Menu helper - Update a menu
     */
    protected function _menu_update($menuName = null, $content = null) {
        try {
            // The menu already exists
            if ($this->_menu_read($menuName, true)) {
                // Update
                $this->sql->query(
                    $this->sql->update(
                        'menus',
                        array(
                            'content' => $this->sql->serialize($content)
                        ),
                        $this->sql->where(
                            array(
                                'name' => $menuName,
                                'lang' => lang()
                            )
                        )
                    )
                );
            }
            // This menu was not yet created
            else {
                $this->sql->query(
                    $this->sql->insert(
                        'menus',
                        array(
                            'name' => $menuName,
                            'content' => $this->sql->serialize($content),
                            'lang' => lang()
                        )
                    )
                );
            }

            return true;
        } catch (Exception $e) {
            return false;
        }
    }// end function _menu_update()

    /**
     * Menu helper - parse a menu into a list
     */
    protected function _menu_parse_to_list($content, $pattern) {
        // Parse the items
        if (isset($content['items']) && isset($content['order'])) {
            $items = array();
            foreach ($content['items'] as $key => $item) {
                // Make it readable
                $items[$key] = sprintf(
                        // id, type, title, url
                        $pattern,
                        $key,
                        preg_replace('/[0-9]*/i', '', $item[2]),
                        $item[1],
                        $item[0]
                );
            }

            return array_structure($items, $content['order']);
        } else {
            return false;
        }
    }// end function _menu_parse_to_list()
    
}
    
/* End Of File <admin.inc> */