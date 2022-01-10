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
 * Public controller
 * 
 * @package OctoMS
 * @subpackage Fervoare CMS
 */
class open extends octoms {

    /**
     * Class constructor
     * 
     */
    public function __construct() {
        // Load the model
        $this->_('open', OMS_M, 'model');
    }// end function __construct()

    /**
     * Print a web page
     * 
     * @param string $pageType - <ul>
     * <li>page</li>
     * <li>index</li>
     * <li>blog_page</li>
     * <li>blog_index</li>
     * </ul>
     * @param string $id - Used with page and blog_page
     * @return null
     */
    public function serve() {
        // Get the arguments
        $args = func_get_args();

        // Get the type
        $type = array_shift($args);

        // Get the id
        if (null === $id = array_shift($args)) {
            // If no id is specified, show the index or blog_index
            $type = str_replace('page', 'index', $type);
        }

        // If the type is blog, redirect to the custom address
        if (strpos($type, 'blog') !== false) {
            // Get the user-defined blog slug
            $blog_slug = $this->settings->get('seo.blog_slug');

            // This is a valid blog page but redirect to the user-defined URL
            if ($blog_slug !== null && $blog_slug !== octoms::$oms_url_segments[0]) {
                // Replace the 'blog' with our custom string
                octoms::$oms_url_segments[0] = $blog_slug;

                // Redirect 
                $this->template->redirect(implode(WS, octoms::$oms_url_segments));
            }
        }

        // Set allowed types for security reasons
        if (!in_array($type, array('index', 'page', 'blog_index', 'blog_page'))) {
            $type = 'page';
        }

        // Load the page template
        $this->template->conf('page', $type);

        // Get the data
        if (
            false !== $data = method_exists($this->model, $type) ?
            call_user_func(
                array(
                    $this->model,
                    $type
                ),
                $id
            ) :
            false
        ) {
            $this->template->title($data['title']);
        } elseif (strpos($type, 'index') === false) {
            $this->error->page(404);
        }

        // Build the menu at startup
        $this->model->menus();

        // Get the options
        if (isset($data['options'])) {
            $options = $this->sql->unserialize($data['options']);

            // Append the javascript
            isset($options['js']['server']) ? $this->template->js($options['js']['server']) : null;
            isset($options['js']['onpage']) ? $this->template->js(array($options['js']['onpage'], 'remote' => false)) : null;

            // Append the CSS
            isset($options['css']['server']) ? $this->template->css($options['css']['server']) : null;
            isset($options['css']['onpage']) ? $this->template->css(array($options['css']['onpage'], 'remote' => false)) : null;

            // Add keywords and description
            isset($options['seo']['keywords']) ? $this->template->meta('keywords', $options['seo']['keywords']) : null;
            isset($options['seo']['description']) ? $this->template->meta('description', $options['seo']['description']) : null;
        }

        // Everything thrown here goes right to the content() $template method
        if (isset($data['content'])) {
            print $this->template->shortcode($data['content']);
        }
    }// end function serve()

    /**
     * Search capabilities
     */
    public function search() {
        // Get the arguments
        $args = func_get_args();

        // Deliver the Open Search Description XML
        if (count($args) == 2 && $args[0] == 'open' && $args[1] == 'description') {
            // The content is xml
            header('Content-type:text/xml');

            // Set this as whitescreen
            $this->template->conf('whitescreen', true);

            // Prepare the description
            $desc = array(
                'short_name' => $this->settings->get('general.site_name'),
                'root' => DOMAIN_PROTOCOL . DOMAIN . baseurl(),
                'i18n_root' => DOMAIN_PROTOCOL . DOMAIN . i18n_baseurl(),
                'tags' => implode(', ', (array) $this->settings->get('seo.site_keywords'))
            );

            // Load the description
            print "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
            print $this->view->load('open/search_description', $desc);
            exit();
        } else {
            // Build the menu at startup
            $this->model->menus();

            // No page provided? Redirect to the first page
            $page = isset($args[1]) ? $args[1] : 1;

            // Get the search term
            $term = urldecode($args[0]);

            // Set the title
            $this->template->title(t(__FILE__, 13, 'Search for "%1"', htmlentities($term)));

            // TODO Get the number of results per page
            $results = 10;

            // Get the results
            $r = (array) $this->model->search($term, $page, $results);

            // Set the default page template
            $this->template->conf('page', 'page');

            // Load the template
            print $this->view->load('open/search_result', array('data' => $r));
        }
    }// end function search()
}

/* End Of File <open.inc> */