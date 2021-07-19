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
    
if (!function_exists('action_btn')) {
    /**
     * Create an action form button
     * 
     * @example 
     * // Submit a form to {this page}/save
     * action_btn('Save',10);
     * // Optionally, use a custom name for the hidden input
     * action_btn('Save',10,'custom_save');
     * 
     * @param string $text
     * @param string $value
     * @param string $name
     * @param array $options
     * @return string - A self-submitting action button
     */
    function action_btn($text = null, $value = null, $name = null, $options = array()) {
        // Add the class
        if (isset($options['class'])) {
            $options['class'] .= ' x';
        } else {
            $options['class'] = 'x';
        }

        // And some styling
        if (isset($options['style'])) {
            $options['style'] .= ';display:inline-block;';
        } else {
            $options['style'] = 'display:inline-block';
        }

        // All done
        return form(null, null, $options) .
            input('hidden', !is_null($name) ? $name : $text, $value) .
            submit(ucfirst($text), false) .
            form(true);
    }
}

if (!function_exists('action_radio')) {
    /**
     * Create a one-button radio form
     * 
     * @example 
     * // Create a self-submitting radio form
     * action_radio('name','value',true,'Title');
     * 
     * @param string $name
     * @param string $value
     * @param boolean $checked
     * @param string $title
     * @param string $options
     * @return string - A self-submitting radio form
     */
    function action_radio($name = null, $value = null, $checked = false, $title = null, $options = array()) {
        // Submit on change
        $options['onClick'] = '$(this.form).submit();';

        // Checked?
        if ($checked) {
            $options['checked'] = 'checked';
        }

        // Use a title?
        if (!is_null($title)) {
            $options['title'] = $title;
        }

        // Styling
        $options['style'] = 'cursor:pointer';

        // Return the form
        return form(null, null, array('class' => 'x', 'style' => 'display:inline-block;')) .
            input('radio', $name, $value, $options) .
            form(true);
    }
}

if (!function_exists('delete_btn')) {
    /**
     * Creates a HTML delete button
     * 
     * @example 
     * // Create a form that sends {"delete":"smth"}
     * delete_btn('smth')
     * // Create a form that sends {"x_delete":"smth"}
     * delete_btn('smth','x_delete');
     * 
     * @param string $value
     * @param string $name
     * @return string - A delete form
     */
    function delete_btn($value = null, $name = null, $options = null) {
        return action_btn('delete', $value, $name, $options);
    }
}

if (!function_exists('link_btn')) {
    /**
     * Create a link button
     * 
     * @example 
     * // Use a button-type link to "abc"
     * link_btn("Go to page","abc");
     * // Use a custom page, not related to this one
     * link_btn("Go home",'/',false);
     * 
     * @param string $text
     * @param string $page
     * @param string $here
     * @return string - The link button
     */
    function link_btn($text = null, $page = null, $here = true) {
        return tag('span', array('class' => 'btn'), a($text, $here ? here($page) : $page));
    }
}

if (!function_exists('t2')) {
    /**
     * Translation helper 2 - search within the nearest i18n folder
     * Uses '{here}/i18n/{language}.inc' files.
     * Format: PHP files with this array: <ul>
     * <li>$t</li>
     * </ul>
     * Example:
     * // This is the translation of "Hello %1!' in romanian (file "i18n/ro.inc")
     * $t[1] = 'Salut %1!';
     * 
     * @example 
     * // Prints "Hello world!"
     * print t2(__FILE__,1,'Hello %1!','world');
     * 
     * @param string $FILE - Use __FILE__ to populate this;
     * @param int $id - The id of the text to translate
     * @param string $text - The text to translate; parameters are accepted; 
     * the parameter format is %{number}
     * @param [,...] string - Parameters for the $text in order
     * @return string - Translated string or the default English text
     */
    function t2($FILE, $id, $text) {
        // Get the folder
        $folder = preg_replace('%(.*)\\' . DS . '.*?\..*%i', '$1', $FILE);

        // Save the configurations
        if (!isset(octoms::$oms_info['i18n'][$folder]) && lang() != 'en') {
            $t = array();
            // Load the language file
            if (file_exists($f = $folder . DS . 'i18n' . DS . lang() . EXT)) {
                require_once $f;
            }
            octoms::$oms_info['i18n'][$folder] = array(
                't' => $t,
            );
            unset($t);
        }

        // Get the string
        if (isset(octoms::$oms_info['i18n'][$folder]['t'][$id])) {
            $text = octoms::$oms_info['i18n'][$folder]['t'][$id];
        }

        // Get the arguments
        global $args;
        $args = array_slice(func_get_args(), 3);

        // Perform the replacement
        $text = preg_replace_callback(
            '/%([0-9]+)/i',
            create_function(
                '$m',
                'global $args;return isset($args[$m[1]-1])?$args[$m[1]-1]:"";'
            ),
            $text
        );

        // Unset the arguments set as global
        unset($args);

        // All done
        return $text;
    }
}

/* End Of File <html.inc> */