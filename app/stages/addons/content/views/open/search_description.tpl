<OpenSearchDescription xmlns="http://a9.com/-/spec/opensearch/1.1/">
  <ShortName><?php print t(__FILE__, 'search_name', '%1 - Search (%2)',$this->v('short_name'),lang());?></ShortName>
  <Description><?php print t(__FILE__, 'search_desc', 'Search for info on %1',$this->v('short_name'));?></Description>
  <Tags><?php print $this->v('tags');?></Tags>
  <Image height="16" width="16" type="image/vnd.microsoft.icon"><?php print $this->v('root');?>favicon.ico</Image>
  <Url type="text/html" template="<?php print $this->v('i18n_root');?>search/{searchTerms}/{startPage}/opensearch" />
  <Query role="example" searchTerms="fervoare" />
</OpenSearchDescription>