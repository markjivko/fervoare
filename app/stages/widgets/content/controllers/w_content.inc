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
 * The content widget
 */
class w_content extends octoms {

    /**
     * Class constructor
     */
    public function __construct() {
        $this->_('view', OMS_CL);
    }// end function __construct()

    /**
     * Display recent posts
     */
    public function recent_posts($page = null, $results_per_page = null) {
        // Select all the posts
        // @TODO set limit for pagination
        $this->sql->query(
            $this->sql->select(
                    null,
                    'posts',
                    $this->sql->where(
                            array(
                                'status' => 1,
                                'type' => 1,
                                'lang' => lang()
                            )
                    )
            ) . ' ORDER BY `modified` DESC');

        if ($this->sql->num_rows() > 0) {
            // First unserialize and decompress the data
            foreach ($rows = $this->sql->rows() as $k => $v) {
                $rows[$k]['content'] = $this->input->abstr($this->input->clean_article($this->sql->decompress($v['content'])));
                $rows[$k]['options'] = $this->sql->unserialize($v['options']);
            }

            // Then load the template
            print $this->view->load('post_excerpt', array('d' => $rows, 'blog' => $this->settings->get('seo.blog_slug')));
        } else {
            print 'No posts yet.';
        }
    }// end function recent_posts()
}
    
/* End Of File <w_content.inc> */