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
 * Settings model
 */
class admin_m extends octoms {

    /**
     * General settings - read
     */
    public function general_read() {
        // Prepare the general settings
        $this->settings->prep('general');

        // Return the values
        return $this->settings->getChildren('general');
    }// end function general_read()

    /**
     * General settings - update
     */
    public function general_update() {
        // Set allowed keys
        $settings = array(
            'site_name',
            'motto',
            'twitter',
            'facebook',
            'wordpress',
            'vimeo',
            'youtube',
            'tumblr',
            'email',
            'phone'
        );

        // Set the values
        foreach ($settings as $a) {
            !is_null($this->input->post($a)) 
                ? $this->settings->set('general.' . $a, $this->input->post($a)) 
                : null;
        }

        // All done!
        $this->template->api('General settings updated successfully.');
    }// end function general_update()

    /**
     * SEO read
     */
    public function seo_read() {
        // Prepare the SEO settings
        $this->settings->prep('seo');

        // Return the values
        return $this->settings->getChildren('seo');
    }// end function seo_read()

    /**
     * SEO update
     */
    public function seo_update() {
        // Set allowed keys
        $settings = array(
            'general_extension',
            'site_title',
            'site_keywords',
            'site_description',
            'blog_title',
            'blog_slug',
            'blog_keywords',
            'blog_description',
            'ga_id'
        );

        // Set the values
        foreach ($settings as $a) {
            if (!is_null($this->input->post($a))) {
                if (strpos($a, 'keywords') !== false) {
                    $this->settings->set('seo.' . $a, explode(',', $this->input->post($a)));
                } else {
                    // Update the extension
                    if ($a == 'general_extension') {
                        // Clean the extension
                        $ext = strtolower(preg_replace('/[^a-zA-Z]/i', '', $this->input->post($a, 'all')));

                        // Valid extension?
                        if (!in_array($ext, array('txt', 'xml'))) {
                            $this->settings->set('seo.general_extension', $ext);
                            $this->template->api(array('refresh'));
                        }
                    }
                    // Update the blog slug
                    elseif ($a == 'blog_slug') {
                        if (false !== $new_slug = $this->_update_blog_slug($this->settings->get('seo.blog_slug'), $this->input->post($a))) {
                            // Set the blog slug
                            $this->settings->set('seo.blog_slug', $new_slug);
                        }
                    } else {
                        // Update the settings
                        $this->settings->set('seo.' . $a, $this->input->post($a));
                    }
                }

                $region = !isset($region) ? substr($a, 0, strpos($a, '_')) : $region;
            }
        }

        // Ajax update
        $this->template
                ->api(array('seo_update'))
                ->api_cnt(
                        $this->settings->getChildren('seo')
        );

        // Which region was updated?
        switch ($region) {
            case 'general':$message = 'General settings updated successfully.';
                break;
            case 'site':$message = 'Site front SEO updated successfully.';
                break;
            case 'blog':$message = 'Blog front SEO updated successfully.';
                break;
            case 'ga':$message = 'Google Analytics tracking code updated successfully.';
                break;
        }
        $this->template->api($message);
    }// end function seo_update()

    /**
     * Update the blog slug - helper
     */
    protected function _update_blog_slug($old_slug = null, $new_slug = null) {
        // The slug is needed
        if (is_null($new_slug)) {
            return false;
        }

        // Clean the new slug
        $new_slug = preg_replace('%[^a-zA-Z0-9_-]%i', '', $new_slug);

        // Do not allow the overwrite of the admin section or a null string
        if (strtolower($new_slug) == 'admin' || strlen($new_slug) == 0)
            return false;

        // Make sure there is no article with this slug
        if ($this->sql->query(
                $this->sql->select(
                    null,
                    'posts',
                    $this->sql->where(
                        array(
                            'slug' => $new_slug,
                            'type' => 0,
                            'lang' => lang()
                        )
                    )
                )
            )->num_rows() > 0)
            return false;

        // Load the routes files
        $c = file_get_contents($f = APP_CONFIG . DS . 'routes' . EXT);

        // Find an earlier save
        if (!is_null($old_slug)) {
            $match = preg_match(sprintf('/\$route\[\'%s\'\]/i', $old_slug), $c);
        } else {
            $match = false;
        }

        // Add a new route?
        if (!$match) {
            $c = preg_replace(
                '%(.*\$route\[\'admin\'\])%i',
                sprintf(
                    "\$route['%s'] = 'dispatch/dispatch/open/blog';\n\t\$route['%s/(:any)'] = 'dispatch/dispatch/open/blog/\\$1';\n\n\$1",
                    $new_slug,
                    $new_slug
                ),
                $c
            );
        }
        // Perform a replacement of the existing route
        else {
            $c = preg_replace(
                sprintf('/\'%s(.*?)\'/i', $old_slug),
                sprintf('\'%s$1\'', $new_slug),
                $c
            );
        }

        // Rewrite the file
        $fh = fopen($f, 'w');
        fwrite($fh, $c, strlen($c));
        fclose($fh);

        // Done
        return $new_slug;
    }// end function _update_blog_slug()

    /**
     * Users - read
     * 
     * @param string $id
     */
    public function users_read($id = null) {
        // Get general info
        if (is_null($id)) {
            // Get information about the other users
            if (
                $this->sql->query(
                    $this->sql->select(
                        null,
                        'users',
                        $this->sql->where(
                            array(
                                'id' => array(
                                    $this->auth->user['id']
                                ),
                                sql_cl::NOT
                            )
                        )
                    )
                )->num_rows() > 0
            ) {
                $info['others'] = $this->sql->rows();
            } else {
                $info = null;
            }
        }
        // Get specific info
        else {
            if (
                $this->sql->query(
                    $this->sql->select(
                        null,
                        'users',
                        $this->sql->where(
                            array(
                                'id' => $this->input->clean_all($id)
                            )
                        )
                    )
                )->num_rows() > 0
            ) {
                $info = $this->sql->first();
            } else {
                $info = null;
            }
        }

        // Return the result
        return $info;
    }// end function users_read()

    /**
     * Users - update
     */
    public function users_update($args = array()) {
        // Delete a user?
        if ($this->input->post('delete')) {
            try {
                // Delete the user from the database
                $this->sql->query(
                    $this->sql->delete(
                        'users',
                        $this->sql->where(
                            array(
                                'id' => $this->input->post('delete', 'all')
                            )
                        )
                    )
                );

                // All done
                $this->template
                    ->api('User #' . $this->input->post('delete') . ' deleted successfully.')
                    ->api(array('delRow'));
            } catch (Exception $e) {
                // Something went wrong
                $this->template->api('Could not delete user #' . $this->input->post('delete'), false);
            }
            return;
        }

        // Log an user off
        if ($this->input->post('log_out')) {
            // Update the log_off_flag for the user
            $this->auth->flag_log_out(true, $this->input->post('log_out', 'all'));

            // All done
            $this->template
                ->api('User #' . $this->input->post('log_out', 'all') . ' logged out successfully.')
                ->api(array('logOut'));
        }

        // Update the information on one user
        if ($this->input->post('username')) {
            try {
                // Prepare the update
                $update = array(
                    'username' => trim($this->input->post('username', 'all')),
                    'role' => $this->input->post('role', 'all'),
                    'name' => trim($this->input->post('name', 'all')),
                    'email' => trim($this->input->post('email', 'all'))
                );

                // Only admins are allowed to change the role
                if ($this->auth->role() != 1)
                    unset($update['role']);

                // Send the data to the user for AJAX update
                $this->template
                    ->api_cnt($update)
                    ->api(array('formUpdate'));

                // Get the id from the arguments list
                if (count($args) > 1) {
                    $id = $args[1];
                }
                // Or from the session
                else {
                    // Get the user id
                    $id = $this->auth->user['id'];

                    // Get the session user information
                    $user = $this->session->get('user');

                    // Force session overwrite
                    foreach ($update as $k => $v)
                        $user[$k] = $v;

                    // Update the info
                    $this->session->set('user', $user);
                }

                // Verify the password before updating
                if ($this->input->post('new_password') != '') {
                    // Not valid? Stop the update; admins can do anything
                    if ($this->auth->role() != 1 && $this->auth->verify($id, $this->input->post('old_password'), true) != $id) {
                        // Stop; no further update possible
                        $this->template->api('The old password you provided does not match our records. The rest of the fields were not saved.', false);
                        return;
                    }
                    // Append the password information to the update array
                    else {
                        $update['password'] = md5($this->input->post('new_password'));
                    }
                }

                // Perform the update
                $this->sql->query(
                    $this->sql->update(
                        'users',
                        $update,
                        $this->sql->where(
                            array(
                                'id' => $id
                            )
                        )
                    )
                );

                // Good
                $this->template->api('User "' . $this->input->post('username') . '" is up to date.');

                // Log out the modified user
                $this->auth->flag_log_out(true, $id);

                // Refresh
                if (count($args) == 0) {
                    $this->template->api(array('formUpdate', 'refresh'));
                }
            } catch (Exception $e) {
                $this->template->api('Could not update the user information. Refresh the page and try again.', false);
            }
        }
    }// end function users_update()

    /**
     * Users - add
     */
    public function users_add() {
        // Get the blank user
        if (
            $this->sql->query(
                $this->sql->select(
                    null,
                    'users',
                    $this->sql->where(
                        array(
                            'username' => '',
                            'password' => ''
                        )
                    )
                )
            )->num_rows() > 0
        ) {
            $row = $this->sql->first();
            return $row['id'];
        }
        // Or create a new one
        else {
            // Insert a new blank user
            $this->sql->query(
                $this->sql->insert(
                    'users',
                    array(
                        'username' => '',
                        'password' => '',
                        'role'     => $this->auth->roles['user'],
                        'created'  => time()
                    )
                )
            );

            // Return the last insert ID
            return $this->sql->insert_id();
        }
    }// end function users_add()
}

/* End Of File <admin.inc> */