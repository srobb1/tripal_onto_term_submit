<?php
$onto_term_submit = $node->onto_term_submit;

// expand the onto_term_submit to include the properties.
$options = array(
  'return_array' => 1,
  'order_by' => array('rank' => 'ASC'),
);
$onto_term_submit = chado_expand_var($onto_term_submit,'table', 'onto_term_submitprop', $options);
$onto_term_submitprops = $onto_term_submit->onto_term_submitprop;
$properties = $node->onto_term_submit->onto_term_submitprop;

if (count($properties)) { ?>
  <div class="tripal_onto_term_submit-data-block-desc tripal-data-block-desc">Below are the terms that are related to <?php print $onto_term_submit->term_name ?></div> <?php

  // the $headers array is an array of fields to use as the column headers.
  // additional documentation can be found here
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $headers = array('Relationship', 'Relation');

  // the $rows array contains an array of rows where each row is an array
  // of values for each column of the table in that row. Additional
  // documentation can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $rows = array();

  $keywords = array();
  foreach ($properties as $property) {
    // each keyword is stored as a separate properties. We want to show them
    // only in a single field not as a bunch of individual properties, so when
    // we see one, save it in an array for later and don't add it yet to the
    // table yet.
    if ($property->type_id->name == 'Keywords') {
      $keywords[] = $property->value->name;
      continue;
    }

if(!isset($property->value->definition)){
 $sql = "select name, definition from {cvterm} cvt where cvterm_id = :cvterm_id";
  $results = chado_query($sql,array(':cvterm_id'=>$property->value->cvterm_id));
  foreach($results as $rt){
    $property->value->definition=$rt->definition;
  }
}


    $rows[] = array(
      '<u>' . $property->type_id->name .'</u><br><i>' . $property->type_id->definition . '</i>',
      '<u>' .$property->value->name   .'</u><br><i>' . $property->value->definition . '</i>',
//      $property->name . '<br><i>' . $property->definition . '</i>'
    );
  }
  // now add in a single row for all keywords
  if (count($keywords) > 0) {
    $rows[] = array(
      'Keywords',
      implode(', ', $keywords),
    );
  }

  // the $table array contains the headers and rows array as well as other
  // options for controlling the display of the table. Additional documentation
  // can be found here:
  // https://api.drupal.org/api/drupal/includes%21theme.inc/function/theme_table/7
  $table = array(
    'header' => $headers,
    'rows' => $rows,
    'attributes' => array(
      'id' => 'tripal_onto_term_submit-table-properties',
      'class' => 'tripal-data-table'
    ),
    'sticky' => FALSE,
    'caption' => '',
    'colgroups' => array(),
    'empty' => '',
  );

  // once we have our table array structure defined, we call Drupal's
  // theme_table() function to generate the table.
  print theme_table($table);
}
