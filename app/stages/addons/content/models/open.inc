<?php if(!defined('OCTOMS')){header('HTTP/1.1 403');die('{"error":"forbidden"}');}
/*
 * @package   Fervoare CMS
 * @copyright (c) 2012 Mark Jivko https://markjivko.com
 * @license   GPL v3.0 (http://www.gnu.org/licenses/gpl-3.0.txt)
 * 
 * Redistributions of files must retain the above copyright notice.
 * 
 * @version   0.0.1
 */
    
/*
 * Open model
 */
class open_m extends octoms {

    /**
     * Index - Serve the site front page
     */
    public function index() {
        // Get page alternatives
        $i18n = array();
        foreach (explode(',', LANGUAGES) as $key => $l) {
            if ($l != lang()) {
                unset($this->settings->data['seo']);
                $this->settings->setLang($l);
                $i18n[$key] = array(
                    'href' => baseurl($l == DEFAULT_LANGUAGE ? '' : $l),
                    'lang' => $l,
                    'title' => $this->settings->get('seo.site_title')
                );
            }
        }
        unset($this->settings->data['seo']);
        $this->settings->setLang(lang());

        // Set the title
        $this->template->title($this->settings->get('seo.site_title'));

        // Set the meta
        $this->template->meta(
                array(
                    'keywords' => $this->settings->get('seo.site_keywords'),
                    'description' => $this->settings->get('seo.site_description'),
                    'i18n' => $i18n
                )
        );
    }// end function index()

    /**
     * Page - serve a standalone web page
     * 
     * @param string $slug - The page slug
     * @return array - Post content OR False on error
     */
    public function page($slug = '') {
        try {
            // Get the posts
            $data = $this->sql->query(
                $this->sql->select(
                    null,
                    'posts',
                    $this->sql->where(
                        array(
                            'slug' => $slug,
                            'type' => 0,
                            'status' => 1,
                            'lang' => lang()
                        )
                    )
                )
            )->first();

            // Get the content
            if (isset($data['content'])) {
                $data['content'] = $this->sql->decompress($data['content']);
            }

            // Get the page equivalents
            $this->_page_eq($data['id'], $data['eq_to']);

            // Return the data
            return $data;
        } catch (Exception $e) {
            flag($e->getMessage());
            return false;
        }
    }// end function page()

    /**
     * Blog index - Serve the site's weblog's front page
     */
    public function blog_index() {
        // Get page alternatives
        $i18n = array();
        foreach (explode(',', LANGUAGES) as $key => $l) {
            if ($l != lang()) {
                unset($this->settings->data['seo']);
                $this->settings->setLang($l);
                $i18n[$key] = array(
                    'href' => baseurl(
                        ($l == DEFAULT_LANGUAGE ? '' : $l . WS) .
                        $this->settings->get('seo.blog_slug')
                    ),
                    'lang' => $l,
                    'title' => $this->settings->get('seo.blog_title')
                );
            }
        }
        unset($this->settings->data['seo']);
        $this->settings->setLang(lang());

        // Set the title
        $this->template->title($this->settings->get('seo.blog_title'));

        // Set the meta
        $this->template->meta(
            array(
                'keywords' => $this->settings->get('seo.blog_keywords'),
                'description' => $this->settings->get('seo.blog_description'),
                'i18n' => $i18n
            )
        );

        // Load the "Recent posts" widget right here
        $this->template->widget('content', 'recent_posts');
    }// end function blog_index()

    /**
     * Blog post
     * 
     * @param string $slug
     * @return array - Post content OR False on error
     * 
     */
    public function blog_page($slug = null) {
        try {
            // Find the blog page
            if (!is_null($slug)) {
                $data = $this->sql->query(
                    $this->sql->select(
                        null,
                        'posts',
                        $this->sql->where(
                            array(
                                'slug' => $slug,
                                'type' => 1,
                                'status' => 1,
                                'lang' => lang()
                            )
                        )
                    )
                )->first();

                // Get the content
                if (isset($data['content'])) {
                    $data['content'] = $this->input->tags_close(
                        $this->sql->decompress(
                            $data['content']
                        )
                    );
                }

                // Get the page equivalents
                $this->_page_eq($data['id'], $data['eq_to']);
            } else {
                // No blog page provided
                $data = false;
            }

            // All done
            return $data;
        } catch (Exception $e) {
            flag($e->getMessage());
            return false;
        }
    }// end function blog_page()

    /**
     * Page equivalents - i18n
     * 
     * @param int $id - post id
     * @param int $eq_to - eq_to DB value
     */
    protected function _page_eq($id = null, $eq_to = null) {
        // The id is a must
        if (is_null($id))
            return;

        // Get the language alternatives
        $eq = array();
        if ('en' == lang()) {
            // Get all the alternatives to this page
            if ($this->sql->query(
                $this->sql->select(
                    null,
                    'posts',
                    $this->sql->where(
                        array(
                            'eq_to' => $id
                        )
                    )
                )
            )->num_rows() > 0) {
                foreach ($this->sql->rows() as $row) {
                    // Get the blog slug
                    if ($row['type'] == '1') {
                        $this->settings->setLang($row['lang']);
                        $blog = $this->settings->get('seo.blog_slug') . WS;
                        $this->settings->setLang();
                    } else {
                        $blog = '';
                    }

                    // Add the findings
                    $eq[] = array(
                        'href' => baseurl(
                            ($row['lang'] == DEFAULT_LANGUAGE ? '' : $row['lang'] . WS) . $blog .
                            $row['slug']
                        ),
                        'lang' => $row['lang'],
                        'title' => $row['title']
                    );
                }
            }
        } else {
            if (strval($eq_to) != '0') {
                // Get all the OTHER pages that are equivalent to the english version
                if ($this->sql->query(
                    $this->sql->select(
                        null,
                        'posts',
                        $this->sql->where(
                            array(
                                'eq_to' => $eq_to
                            ),
                            array(
                                'lang' => lang(),
                                sql_cl::NOT
                            )
                        )
                    )
                )->num_rows() > 0) {
                    foreach ($this->sql->rows() as $row) {
                        // Get the blog slug
                        if ($row['type'] == '1') {
                            $this->settings->setLang($row['lang']);
                            $blog = $this->settings->get('seo.blog_slug') . WS;
                            $this->settings->setLang();
                        } else {
                            $blog = '';
                        }

                        // Add the findings
                        $eq[] = array(
                            'href' => baseurl(
                                ($row['lang'] == DEFAULT_LANGUAGE ? '' : $row['lang'] . WS) . $blog .
                                $row['slug']
                            ),
                            'lang' => $row['lang'],
                            'title' => $row['title']
                        );
                    }
                }

                // Get the english page
                if (
                    $this->sql->query(
                        $this->sql->select(
                            null,
                            'posts',
                            $this->sql->where(
                                array(
                                    'id' => $eq_to
                                )
                            )
                        )
                    )->num_rows() > 0
                ) {
                    // Get the row
                    $row = $this->sql->first();

                    // Get the blog slug
                    if ($row['type'] == '1') {
                        $this->settings->setLang('en');
                        $blog = $this->settings->get('seo.blog_slug') . WS;
                        $this->settings->setLang();
                    } else {
                        $blog = '';
                    }

                    $eq[] = array(
                        'href' => baseurl(
                                $blog . $row['slug']
                        ),
                        'lang' => $row['lang'],
                        'title' => $row['title']
                    );
                }
            }
        }

        // Valid results?
        if (count($eq) > 0) {
            // Set the alternatives
            $this->template->meta(
                    array(
                        'i18n' => $eq
                    )
            );
        }
    }// end function _page_eq()

    /**
     * Generate the menu
     * 
     * @param array $options
     * @return boolean - True on Success, False on Failure
     */
    public function menus($options = array()) {
        try {
            // Get the menu
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

            // Found somethings?
            if ($this->sql->num_rows() > 0) {
                foreach ($this->sql->rows() as $row) {
                    if (false !== $menu = $this->_menu_parse_to_list(
                            $this->sql->unserialize($row['content'])
                            )) {
                        $this->template->menu(
                                $row['name'],
                                $menu,
                                true
                        );
                    }
                }
                return true;
            } else {
                return false;
            }
        } catch (Exception $e) {
            flag($e->getMessage());
            return false;
        }
    }// end function menus()

    /**
     * Menu helper - parse a menu into a list
     * 
     * @param array $content - (items=>array(),order=>array())
     * @return array|false - False on failure
     */
    protected function _menu_parse_to_list($content) {
        // Parse the items
        if (isset($content['items']) && isset($content['order'])) {
            // Initialize the items
            $items = array();

            // Parse the items
            foreach ($content['items'] as $key => $item) {
                // Current page?
                $current = $item[0] == here() ? '[*]' : '';

                // This is a link
                $items[$key] = $current . a($item[1], $item[0]);
            }

            // All done
            return array_structure($items, $content['order']);
        } else {
            return false;
        }
    }// end function _menu_parse_to_list()

    /**
     * Search
     * 
     * @param string $term - Search term
     * @param int $page - Page number
     * @param int $results - Results per page
     */
    public function search($term = '', $page = 1, $results = 10) {
        // No results if no term
        if (strlen($term) == 0) {
            return null;
        }

        // Search the database
        $limit = (($page - 1) * $results) . ',' . $results;

        // Get the blog slug
        $blog = $this->settings->get('seo.blog_slug');

        try {
            $replacementMap = array(
                "\0" => "\\0",
                "\n" => "\\n",
                "\r" => "\\r",
                "\t" => "\\t",
                chr(26) => "\\Z",
                chr(8) => "\\b",
                '"' => '\"',
                "'" => "\'",
                '_' => "\_",
                "%" => "\%",
                '\\' => '\\\\'
            );

            // Get the result
            $this->sql->query(
                $this->sql->select(
                    null,
                    'posts',
                    $this->sql->where(
                        array(
                            'title' => '%' . strtr($term, $replacementMap) . '%',
                            sql_cl::LIKE
                        ),
                        array(
                            'lang' => lang()
                        )
                    ) . ' LIMIT ' . $limit
                )
            );

            // Get the rows
            if ($this->sql->num_rows() > 0) {
                // Prepare the result array
                $result = array();

                // Parse the results
                foreach ($this->sql->rows() as $row) {
                    $result[] = array(
                        'title' => $row['title'],
                        'href' => i18n_baseurl($row['type'] == 0 ? $row['slug'] : ($blog . WS . $row['slug'])),
                        'abstract' => preg_replace(
                                '/\{[^}]*\}/i',
                                '',
                                $this->input->abstr(
                                        $this->sql->decompress(
                                                $row['content']
                                        ),
                                        500
                                )
                        )
                    );
                }

                // All done
                return $result;
            } else {
                return null;
            }
        } catch (Exception $e) {
            flag($e->getMessage());
            return false;
        }
    }// end function search()
}

/* End Of File <open.inc> */