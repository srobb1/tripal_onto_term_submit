<?php

/**
 * @file
 * This file should contain all Drupal hooks for interacting with nodes.
 *
 */

/**
 *  Implementation of hook_node_info().
 *
 *  This hook provides information to Drupal about any node types that are being
 *  created by this module. If your module does not create any node types then
 *  this function is not required.
 *
 * @ingroup tripal_onto_term_submit
 */
function tripal_onto_term_submit_node_info() {
  $nodes = array();

  // EXPLANATION: this array describes all of the node types that are created
  // by this module. For many Tripal modules (e.g. tripal_onto_term_submit, tripal_stock,
  // tripal_library, tripal_pub, etc.) new node types are created. It is
  // customary to name all new node types that interact with data in Chado
  // with a 'chado_' prefix.

  $nodes['chado_onto_term_submit'] = array(
    'name'        => t('Ontology Term Submission'),
    'base'        => 'chado_onto_term_submit',
    'description' => t('A record from the fake chado onto_term_submit table'),
    'has_title'   => TRUE,
    'locked'      => TRUE,
    // EXPLANATION: This section of the node type array specifies how Tripal
    // will sync the node types with data in Chado. When Drupal creates a node
    // it has no way of coordinating which node belongs to which record in
    // Chado. Therefore, Tripal maintains tables in the Drupal schema that maps
    // Drupal nodes to records in Chado. Syncing is the process of creating
    // Drupal nodes and linking them to the appropriate record.
    'chado_node_api' => array(
      // the base table name (e.g. onto_term_submit, onto_term_submit, contact)
      'base_table' => 'onto_term_submit',
      // the node type hook prefix
      'hook_prefix' => 'chado_onto_term_submit',
      'record_type_title' => array(
        // how to refer to the record
        'singular' => t('Ontology Term Submission'),
        // how to refer to the record in plurals
        'plural' => t('Ontology Term Submissions')
      ),
      'sync_filters' => array(
        'type_id' => TRUE,     // if the record has a type_id set to TRUE
        'organism_id' => TRUE  // if the record has an organism_id set to TRUE
      ),
    )
  );

  return $nodes;
}

/**
 * Implement hook_access(). This hook provides instructions to Drupal for which
 * users can access the custom content types created in the function above. The
 * available permissions are set in the chado_onto_term_submit_permissions() hook in the
 * tripal_onto_term_submit.module file. This hook is not needed if no node types were
 * defined in the hook_node_info() hook.
 *
 * @return
 * This function should return null if it does not specifically deny access.
 * This allows for other mechanisms to to deny or reject access. If the return
 * value is TRUE then access is granted regardless of any other rules that might
 * be implemented by other modules.
 */
function tripal_onto_term_submit_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  // EXPLANATION:  in the tripal_onto_term_submit_permissions() function we created the
  // permission types that are used here to check for access permissions to the
  // 'chado_exmaple' node type.
  if($node_type == 'chado_onto_term_submit') {
    if ($op == 'create') {
      if (!user_access('create chado_onto_term_submit content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_onto_term_submit content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_onto_term_submit content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_onto_term_submit content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
  }
  return NODE_ACCESS_IGNORE;
}

/**
 * Implementation of hook_form()
 *
 * Creates the form for editing or inserting a record
 *
 * @ingroup tripal_onto_term_submit
 */
function chado_onto_term_submit_form($node, &$form_state) {
  // EXPLANATION: This function should construct a form array that is used by
  // Drupal to construct a form for inserting or editing our new node type.
  // See this page for information about the Form API:
  // https://api.drupal.org/api/drupal/includes!form.inc/group/form_api/7
  //
  // The code below is laid out in the following order
  // 1) Set default values
  // 2) Add form elements used by this node type
  // 3) Use the Tripal API to add form elements for properties,
  //    dbxref's and relationships
  //
  // For the onto_term_submit code below we assume that the fake 'onto_term_submit' table only has
  // a uniquename, organism_id, type_id and onto_term_submit_id.

  global $user;
  $form = array();
  $admin_rid = user_role_load_by_name('administrator')->rid; // admin want to also add ontology_curator
 // $onto_curator_rid = user_role_load_by_name('ontology_curator')->rid; // admin want to also add ontology_curator


  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object. This occurs when editing an existing
  //    onto_term_submit
  // 2) in the $form_state['values'] array which occurs on a failed validation
  //    or ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from
  //    submit form elements and the form is being rebuilt
  //
  // set form field defaults


  // SET FORM DEFAULTS
  //---------------------------------------------
  $onto_term_submit      = null; // holds the onto_term_submit object record
  $onto_term_submit_id   = null; // when editing an onto_term_submit record we'll have an onto_term_submit_id

  // initialize the defaults for the form fields
  $term_name   = '';
  $definition   = '';
  $definition_ref   = '';
  $term_xref   = '';
  $onto_term_submit_type = '';
  $seealso_url  = '';
  $contact_name  = '';
  $contact_email = '';
  $comments = '';
  $status = '';
  $status_notes= '';

  // if we are editing an existing node then the 'onto_term_submit' record from Chado
  // is already part of the node, so we set the defaults from that object
  if (property_exists($node, 'onto_term_submit')) {
    $onto_term_submit = $node->onto_term_submit;
    $onto_term_submit_id   = $onto_term_submit->onto_term_submit_id;
    $term_name   = $onto_term_submit->term_name;
    $definition  = $onto_term_submit->definition;
    $definition_ref  = $onto_term_submit->definition_ref;
    $term_xref  = $onto_term_submit->term_xref;
    $onto_term_submit_type = $onto_term_submit->type_id->cv_id;
  $seealso_url  = $onto_term_submit->seealso_url;
  $contact_name  = $onto_term_submit->contact_name;
  $contact_email = $onto_term_submit->contact_email;
  $comments = $onto_term_submit->comments;
  $status = $onto_term_submit->status;
  $status_notes = $onto_term_submit->status_notes;

    // keep track of the onto_term_submit id
    $form['onto_term_submit_id'] = array(
      '#type' => 'value',
      '#value' => $onto_term_submit_id,
    );
  }
  // if we are re constructing the form from a failed validation or ajax
  // callback then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
     $term_name   = $form_state['values']['term_name'];
     $definition  = $form_state['values']['definition'];
     $definition_ref  = $form_state['values']['definition_ref'];
     $term_xref  = $form_state['values']['term_xref'];
     $onto_term_submit_type = $form_state['values']['onto_term_submit_type'];
     $seealso_url  = $form_state['values']['seealso_url'];
     $contact_name  = $form_state['values']['contact_name'];
     $contact_email  = $form_state['values']['contact_email'];
     $comments  = $form_state['values']['comments'];
     $status  = $form_state['values']['status'];
     $status_notes  = $form_state['values']['status_notes'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
     $term_name   = $form_state['input']['term_name'];
     $definition  = $form_state['input']['definition'];
     $definition_ref  = $form_state['input']['definition_ref'];
     $term_xref  = $form_state['input']['term_xref'];
     $onto_term_submit_type = $form_state['input']['onto_term_submit_type'];
     $seealso_url  = $form_state['input']['seealso_url'];
     $contact_name  = $form_state['input']['contact_name'];
     $contact_email  = $form_state['input']['contact_email'];
     $comments  = $form_state['input']['comments'];
     $status  = $form_state['input']['status'];
     $status_notes  = $form_state['input']['status_notes'];
  }


  // FORM ELEMENTS
  //---------------------------------------------
  $form['term_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Term Name'),
    '#required' => TRUE,
    '#default_value' => $term_name,
    '#description' => t('Enter a new term name. If you want to suggest changes to an existing term, type existing term name followed by a star(*). For example, if you want to edit any part of the term "head" type head*'),
    '#maxlength' => 255
  );

  // for the type_id we want to use the default vocabulary so that this field
  // can have auto-complete functionality
  $cv_id = $onto_term_submit_type;
     $sql = "SELECT distinct cv.cv_id , cv.name
         FROM {cv} cv
         WHERE name in ('Planarian_Anatomy' , 'Schmidtea_mediterranea_Developmental_Terms' , 'Schmidtea_mediterranea_Developmental_Stages')
         ORDER BY cv.name";

    //$results = chado_query($sql,array(':cv_id' => $cv_id));
    $results = chado_query($sql);
    $libraries[] = '';
    foreach ($results as $library) {
      $libraries[$library->cv_id] = $library->name;
    }
  $form['onto_term_submit_type'] = array(
    '#title'       => t('Term Library'),
    '#type'        => t('select'),
    '#description' => t("Choose the library for which you would like to add a term."),
    '#required'    => TRUE,
    '#options' => $libraries,
    '#default_value' => $onto_term_submit_type
  );
  $form['term_xref'] = array(
    '#type' => 'text_format',
    '#title' => t('Term Reference'),
    '#required' => TRUE,
    '#default_value' => $definition_ref,
    '#description' => t('Where was this term first published? Enter a PMID or a Ontology ID, i.e, UBERON:0000955, for the term. If a PMID does not exist, enter a citation. More than one PMID, Ontology ID,  or citation can be included.'),
    '#format' => 'plain_text'
  );

  $form['definition'] = array(
    '#type' => 'text_format',
    '#title' => t('Definition'),
    '#required' => TRUE,
    '#default_value' => $definition,
    '#description' => t('Enter a definition for this term.'),
    '#format' => 'plain_text'

  ); 

  $form['definition_ref'] = array(
    '#type' => 'text_format',
    '#title' => t('Definition Reference'),
    '#required' => TRUE,
    '#default_value' => $definition_ref,
    '#description' => t('Where was this definition first published? Enter a PMID, Ontology ID, or citation for this term. More than one can be included.'),
    '#format' => 'plain_text'
  ); 

  $form['seealso_url'] = array(
    '#type' => 'textfield',
    '#title' => t('More Informtion URL'),
    '#default_value' => $seealso_url,
    '#description' => t('Enter a URL for more rich information for this term.  This page could contain images, suppplemenatry figures, or a more indepth write up. The URL should be on site you control, i.e, GitHub Repository or Lab Website. An example: https://planosphere.stowers.org/stage5'),
    '#maxlength' => 255
  );

  $form['contact_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Your Name'),
    '#required' => TRUE,
    '#default_value' => $contact_name,
    '#description' => t('Enter your name.'),
  );

  $form['contact_email'] = array(
    '#type' => 'textfield',
    '#title' => t('Your E-mail address'),
    '#required' => TRUE,
    '#default_value' => $contact_email,
    '#description' => t('Enter your email address for us to contact you about this submission.'),
  );

  $form['comments'] = array(
    '#type' => 'text_format',
    '#title' => t('Any Comments'),
    '#default_value' => $comments,
    '#description' => t('Enter any comments about this term or submission. If a relationship (see below) term does not exist, add it here, denoted with a star(*). For example, NewTerm eats* brain. "NewTerm" is your new term name, "eats*" is the new relationship term, and "brain" is a term that is being related to your term. If the related term does not exist, add it here, denoted with a star(*). For example, NewTerm is_a NewRelationTerm*. Make sure to create a submission for any new relationship and relation terms.' ),
    '#format' => 'plain_text'
  );

if (isset($user->roles[$admin_rid]) or isset($user->roles[$onto_curator_rid])){
  $form['status'] = array(
    '#type' => t('select'),
    '#title' => t('Status'),
    '#options' => array('Requested','Reviewing', 'Committed', 'Contacted Author', 'Incomplete', 'Issues', 'Done'),
    //'#options' => array(0 => 'Requested',1 => 'Reviewing',2 => 'Committed',3 => 'Contacted Author',4 => 'Incomplete',5 => 'Issues',6 => 'Done'),
    '#default_value' => $status,
    '#description' => t('Select the appropriate term to indicate the workflow progress.'),

  );
  $form['status_notes'] = array(
    '#type' => 'text_format',
    '#title' => t('Status Notes'),
    '#default_value' => $status_notes,
    '#description' => t('Enter any notes about workflow progress.'),
    '#format' => 'plain_text'
  );
// $form['status_notes']['format']['#access'] = FALSE;
}
     $sql = "SELECT distinct cvt.cvterm_id , cvt.name
         FROM {cvterm} cvt, {cv} cv
         WHERE cv.name in ('Planarian_Anatomy' , 'Schmidtea_mediterranea_Developmental_Terms' , 'Schmidtea_mediterranea_Developmental_Stages')
           AND cvt.cv_id = cv.cv_id
         ORDER BY cvt.name";
    $results = chado_query($sql);
    $all_terms[] = '';
    foreach ($results as $rt) {
      $all_terms[$rt->cvterm_id] = $rt->name;
    }


     $sql = "SELECT distinct cvt.cvterm_id , cvt.name
         FROM {cvterm} cvt, {cvterm} cvt2,  {cv} cv, {cvterm_relationship} cr
         WHERE cv.name in ('Planarian_Anatomy' , 'Schmidtea_mediterranea_Developmental_Terms' , 'Schmidtea_mediterranea_Developmental_Stages')
            AND cvt2.cv_id = cv.cv_id
            AND cr.object_id = cvt2.cvterm_id
            AND cr.type_id = cvt.cvterm_id
         ORDER BY cvt.name";
    $results = chado_query($sql);
    $all_relations[] = '';
    foreach ($results as $rr) {
      $all_relations[$rr->cvterm_id] = $rr->name;
    }


  // PROPERTIES FORM
  //---------------------------------------------
  // If there is a onto_term_submitprop table and you want to allow users to add/remove
  // entries from it through your node form then add this section to your own
  // node form
  //$prop_cv = tripal_get_default_cv('onto_term_submitprop', 'type_id');
  //$cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    // the name of the prop table
    'property_table' => 'onto_term_submitprop',
    // the value of onto_term_submit_id for this record
    'chado_id' => $onto_term_submit_id,
    'select_options' => $all_relations,
    'fieldset_title' => 'Add Relationships',
    'additional_instructions' => 'Additional Instructions Here',
    'value_options' => $all_terms
 
    // the cv.cv_id of the cv governing onto_term_submitprop.type_id
    //'cv_id' => $cv_id
  );
  // Adds the form elements to your current form
  tripal_onto_term_submit_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  // If there is a onto_term_submit_dbxref table and you want to allow users to
  // add/remove entries from it through your node form then add this section to
  // your own node form
/*
  $details = array(
    // the name of the _dbxref table
    'linking_table' => 'onto_term_submit_dbxref',
    // the name of the key in your base chado table
    'base_foreign_key' => 'onto_term_submit_id',
    // the value of onto_term_submit_id for this record
    'base_key_value' => $onto_term_submit_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_dbxrefs($form, $form_state, $details);
*/
  // RELATIONSHIPS FORM
  //---------------------------------------------
  // If there is a onto_term_submit_relationship table and you want to allow users to
  // add/remove entries from it through your node form then add this section to
  // your own node form
/*
  $rels_cv = tripal_get_default_cv('onto_term_submit_relationship', 'type_id');
  $cv_id = $rels_cv ? $rels_cv->cv_id : NULL;
  $details = array(
    // the name of the _relationship table
    'relationship_table' => 'onto_term_submit_relationship',
    // the name of your chado base table
    'base_table' => 'onto_term_submit',
    // the name of the key in your base chado table
    'base_foreign_key' => 'onto_term_submit_id',
    // the value of onto_term_submit_id for this record
    'base_key_value' => $onto_term_submit_id,
    // the human-readable name of your node type
    'nodetype' => 'onto_term_submit',
    // the cv.cv_id of the cv governing onto_term_submit_relationship.type_id
    'cv_id' => $cv_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_relationships($form, $form_state, $details);

  // return the form
*/
 $form['#after_build'][] = 'tripal_onto_term_submit_customize_comment_form';
 $form['definition']['#format'] = 'plain_text';
 // $form['definition']['format']['format']['#access']=FALSE;
  return $form;
}

/**
 * Implementation of hook_validate
 *
 * This function validates a form prior to insert or update. If an error is
 * detected, it sets the error using form_set_error() which takes the user back
 * to the form to make corrections.
 *
 * This validation is being used for three activities:
 *   CASE A: Update a node that exists in both Drupal and Chado
 *   CASE B: Synchronizing a node from Chado to Drupal
 *   CASE C: Inserting a new node that exists in neither Drupal nor Chado
 *
 * @param $node
 *
 *
 * @ingroup tripal_onto_term_submit
 */


function tripal_onto_term_submit_customize_comment_form(&$form) {
  foreach (array('definition', 'term_xref' , 'definition_ref' , 'comments' , 'status_notes') as $field){

    $form[$field]['format']['format']['#access']=FALSE;
    $form[$field]['format']['format']['#default_value']='plain_text';
    $form[$field]['format']['help']['#access']=FALSE;
    $form[$field]['format']['guidelines']['#access']=FALSE;
  }
  //$form['defintion']['und'][0]['format']['#access'] = FALSE;
  return $form;
}


function chado_onto_term_submit_validate($node, $form, &$form_state) {
  // We only want to validate when the node is saved.
  // Since this validate can be called on AJAX and Deletion of the node
  // we need to make this check to ensure queries are not executed
  // without the proper values.
  if(property_exists($node, "op") and $node->op != 'Save') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a onto_term_submit_id. We
  // don't need to validate during syncing so just skip it.
  if (!property_exists($node, 'nid') and property_exists($node, 'onto_term_submit_id') and $node->onto_term_submit_id != 0) {
    return;
  }

  // be sure to always trim text fields
  $node->term_name   = property_exists($node, 'term_name') ? trim($node->term_name) : '';

  // Validating for an update. If the 'nid' property is present in the node then
  // this is an update and validation can be different for updates
  if (property_exists($node, 'nid')) {

    // make sure the onto_term_submit type is an allowed term
    //$type_cv = tripal_get_default_cv('onto_term_submit', 'type_id');
    //$type = tripal_get_cvterm(array(
     // 'name' => $node->onto_term_submit_type,
     // 'cv_id' => $type_cv->cv_id,
   // ));
   // if (!$type) {
    //  form_set_error('onto_term_submit_type', t("The onto_term_submit type is not a valid name from the Sequence Ontology."));
   // }

    // TODO: also we should check that the unique constraint is not invalidated
    // by changing either the type_id, organism_id or uniquename.
  }
  // Validating for an insert
  else {
    // make sure the onto_term_submit type is an allowed term
    //$type_cv = tripal_get_default_cv('onto_term_submit', 'type_id');
   // $type = tripal_get_cvterm(array(
     // 'name' => $node->onto_term_submit_type,
    //  'cv_id' => $type_cv->cv_id,
   // ));
    //if (!$type) {
   //   form_set_error('onto_term_submit_type', t("The onto_term_submit type is not a valid name from the Sequence Ontology."));
   // }

    // TODO: also we should check that the unique constraint doesn't already exist
  }
}

/**
 *  Implementation of hook_insert(). This function is called after the node is
 * inserted into the database. We need it so that we can insert appropriate
 * fields as provided by the user into the database. And so that we can link the
 * new Drupal node to the data in Chado via the chado_onto_term_submit linking table. We
 * can get to this function also during "syncing".
 * With syncing, however, the data already exists in Chado and we do not want
 * to try to re-add it. But we do need to add an entry to the chado_onto_term_submit
 * table to link the Drupal node with the data in the 'onto_term_submit' table of Chado.
 *
 *  This function is not required if the hook_node_info() does not define
 *  any custom node types.
 *
 * @ingroup tripal_onto_term_submit
 */
function chado_onto_term_submit_insert($node) {
  $onto_term_submit_id = '';

  // if there is an onto_term_submit_id in the $node object then this must be a sync so
  // we can skip adding the onto_term_submit as it is already there, although we do need
  // to proceed with insertion into the chado/drupal linking table.
  if (!property_exists($node, 'onto_term_submit_id')) {

    // be sure to always trim text fields
    $node->term_name   = trim($node->term_name);
    //$node->description  = property_exists('description', $node) : trim($node->description['value']) ? '';

    // get the onto_term_submit type record
    $type_cv = tripal_get_default_cv('onto_term_submit', 'type_id');
    $type = tripal_get_cvterm(array(
        'cv_id' => $node->onto_term_submit_type,
       // 'cv_id' => $type_cv->cv_id,
    ));

    // perform the insert using the chado_insert_record function();
    $values = array(
      'term_name' => $node->term_name,
      'definition' => $node->definition['value'],
      'type_id' => $node->onto_term_submit_type,
      'definition_ref' => $node->definition_ref['value'],
      'term_xref' => $node->term_xref['value'],
      'seealso_url' => $node->seealso_url,
      'contact_name' => $node->contact_name,
      'contact_email' => $node->contact_email,
      'comments' => $node->comments['value'],
      'status' => $node->status,
      'status_notes' => $node->status_notes['value'],
    );
    $onto_term_submit = chado_insert_record('onto_term_submit', $values);
    if (!$onto_term_submit) {
      drupal_set_message(t('Unable to add onto_term_submit.'), 'warning');
      tripal_report_error('tripal_onto_term_submit', TRIPAL_WARNING, 'Insert onto_term_submit: Unable to create onto_term_submit where values: %values',
        array('%values' => print_r($values, TRUE)));
      return;
    }

    // get the onto_term_submit_id for linking Drupal node with Chado data
    $onto_term_submit_id = $onto_term_submit['onto_term_submit_id'];

    // Only add to other Chado tables if the base record was inserted properly
    if ($onto_term_submit_id > 0) {

      // If you implemented the properties form in chado_onto_term_submit_form then you
      // need to handle inserting these properties into your Chado prop table.
      $details = array(
        // the name of the prop table
        'property_table'   => 'onto_term_submitprop',
        // the name of your Chado base table
        'base_table'       => 'onto_term_submit',
        // the name of the key in your base table
        'foreignkey_name'  => 'onto_term_submit_id',
        // the value of the onto_term_submit_id key
        'foreignkey_value' => $onto_term_submit_id
      );
      chado_update_node_form_properties($node, $details);
/*
      // If you implemented the dbxrefs form in chado_onto_term_submit_form then you need
      // to handle inserting these database references into your Chado _dbxref
      // table.
      $details = array(
        // the name of your _dbxref table
        'linking_table'    => 'onto_term_submit_dbxref',
        // the name of the key in your base table
        'foreignkey_name'  => 'onto_term_submit_id',
        // the value of the onto_term_submit_id key
        'foreignkey_value' => $onto_term_submit_id
      );
      chado_update_node_form_dbxrefs($node, $details);

      // If you implemented the relationships form in chado_onto_term_submit_form then
      // you need to handle inserting these relationships into your Chado
      // _relationship table.
      $details = array(
        // name of the _relationship table
        'relationship_table' => 'onto_term_submit_relationship',
        // value of the onto_term_submit_id key
        'foreignkey_value' => $onto_term_submit_id
      );
      chado_update_node_form_relationships($node, $details);
 */ 
   }
  }
  else {
    // the node has an onto_term_submit_id so get it for linking Drupal node with Chado
    // data
    $onto_term_submit_id = $node->onto_term_submit_id;
  }

  // Make sure the entry for this onto_term_submit doesn't already exist in the
  // chado_onto_term_submit table if it doesn't exist then we want to add it.
   $check_org_id = chado_get_id_from_nid('onto_term_submit', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->onto_term_submit_id = $onto_term_submit_id;
    drupal_write_record('chado_onto_term_submit', $record);
  }
}

/**
 * Implementation of hook_update(). This function runs after the node has been
 * inserted into the Drupal schema and allows us to update the record in Chado.
 *
 * This function is not required if the hook_node_info() does not define any
 * custom node types.
 *
 * @ingroup tripal_onto_term_submit
 */
function chado_onto_term_submit_update($node) {
  // be sure to always trim text fields
  $node->term_name   = trim($node->term_name);
  $node->definition  = trim($node->definition['value']);
  $onto_term_submit_id = chado_get_id_from_nid('onto_term_submit', $node->nid) ;
  // use the chado_update_record() function to update the record
  $match = array(
    'onto_term_submit_id' => $onto_term_submit_id,
  );
  $values = array(
    'term_name' => $node->term_name,
  );
  $options = array('return_record' => TRUE);
  $update_status = chado_update_record('onto_term_submit', $match, $values, $options);

  if (!$update_status) {
    drupal_set_message(t('Unable to update onto_term_submit.'), 'warning');
    tripal_report_error('tripal_onto_term_submit', TRIPAL_WARNING, 'Update onto_term_submit: Unable to update onto_term_submit where values: %values',
      array('%values' => print_r($values, TRUE)));
  }

  // If you implemented the properties form in chado_onto_term_submit_form then you need
  // to handle updating these properties into your Chado prop table.
  $details = array(
    'property_table' => 'onto_term_submitprop', // the name of the prop table
    'base_table' => 'onto_term_submit',         // the name of your Chado base table
    'foreignkey_name' => 'onto_term_submit_id', // the name of the key in your base table
    'foreignkey_value' => $onto_term_submit_id  // the value of the onto_term_submit_id key
  );
  chado_update_node_form_properties($node, $details);
/*
  // If you implemented the dbxrefs form in chado_onto_term_submit_form then you need to
  // handle updating these database references into your Chado _dbxref table.
  $details = array(
    'linking_table' => 'onto_term_submit_dbxref', // the name of your _dbxref table
    'foreignkey_name' => 'onto_term_submit_id',   // the name of the key in your base table
    'foreignkey_value' => $onto_term_submit_id    // the value of the onto_term_submit_id key
  );
  chado_update_node_form_dbxrefs($node, $details);

  // If you implemented the relationships form in chado_onto_term_submit_form then you
  // need to handle updating these relationships into your Chado _relationship
  // table.
  $details = array(
    // name of the _relationship table
    'relationship_table' => 'onto_term_submit_relationship',
    // value of the onto_term_submit_id key
    'foreignkey_value' => $onto_term_submit_id
  );
  chado_update_node_form_relationships($node, $details);
*/
}
/**
 * Implementation of hook_delete(). This function runs after the node has been
 * deleted from the Drupal schema and allows us to delete the corresponding
 * record in Chado.
 *
 * This function is not required if the hook_node_info() does not define any
 * custom node types.
 *
 * @ingroup tripal_onto_term_submit
 */
function chado_onto_term_submit_delete($node) {

  // get the onto_term_submit id from the node
  $onto_term_submit_id  = chado_get_id_from_nid('onto_term_submit', $node->nid);

  // if we don't have a onto_term_submit id for this node then this isn't a node of type
  // chado_onto_term_submit or the entry in the chado_onto_term_submit table was lost.
  if (!$onto_term_submit_id) {
    return;
  }

  // remove the entry in the chado_exapmle table linking the deleted
  // Drupal node with the data in Chado
  $sql_del = "DELETE FROM {chado_onto_term_submit} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from onto_term_submit tables of Chado database. This will
  // cause a cascade delete and remove all data in referencing tables
  // for this onto_term_submit
  chado_query("DELETE FROM {onto_term_submit} WHERE onto_term_submit_id = :onto_term_submit_id", array(':onto_term_submit_id' => $onto_term_submit_id));

  // inform the user that the data was deleted
  drupal_set_message(t("The onto_term_submit and all associated data were removed from Chado"));

}

/**
 * Implementation of hook_load(). This function is necessary to load into the
 * $node object the fields of the table form Chado. For onto_term_submit for the onto_term_submit
 * table, the chado_onto_term_submit_load() function adds in a onto_term_submit object which
 * contains all of the fields and sub objects for data in tables with foreign
 * key relationships.
 *
 * This function is not required if the hook_node_info() does not define any
 * custom node types.
 *
 * @ingroup tripal_onto_term_submit
 */
function chado_onto_term_submit_load($nodes) {
  // EXPLANATION: when displaying or node or accessing the node in a template
  // we need the data from Chado. This function finds the record in Chado that
  // this node belongs to and adds the record.

  // there may be multiple nodes that get passed in so we have to iterate
  // through them all
  foreach ($nodes as $nid => $node) {
    // find the onto_term_submit and add in the details
    $onto_term_submit_id = chado_get_id_from_nid('onto_term_submit', $nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$onto_term_submit_id) {
      continue;
    }

    // build the onto_term_submit variable by using the chado_generate_var() function
    $values = array('onto_term_submit_id' => $onto_term_submit_id);
    $onto_term_submit = chado_generate_var('onto_term_submit', $values);

    // for fields in the table that are of type 'text' you may want to include
    // those by default, the chado_generate_var does not include text fields as
    // they may be very large and including a large text field can slow the page
    // load.
    // If you know a text field will never be large and it is important for the
    // other functions that will see the node to have access to a field you can
    // include it here using the chado_expand_var() function. In most
    // cases it is probably best to let the end-user decide if text fields
    // should be included by using this function in the templates.
    $onto_term_submit = chado_expand_var($onto_term_submit, 'field', 'onto_term_submit.definition');
    $onto_term_submit = chado_expand_var($onto_term_submit, 'field', 'onto_term_submit.definition_ref');
    $onto_term_submit = chado_expand_var($onto_term_submit, 'field', 'onto_term_submit.term_xref');
    $onto_term_submit = chado_expand_var($onto_term_submit, 'field', 'onto_term_submit.seealso_url');
    $onto_term_submit = chado_expand_var($onto_term_submit, 'field', 'onto_term_submit.contact_name');
    $onto_term_submit = chado_expand_var($onto_term_submit, 'field', 'onto_term_submit.contact_email');
    $onto_term_submit = chado_expand_var($onto_term_submit, 'field', 'onto_term_submit.comments');
    $onto_term_submit = chado_expand_var($onto_term_submit, 'field', 'onto_term_submit.status_notes');
    $onto_term_submit = chado_expand_var($onto_term_submit, 'table', 'onto_term_submitprop', array('return_array' => TRUE));

    // add the new onto_term_submit object to this node.
    $nodes[$nid]->onto_term_submit = $onto_term_submit;

    // If your module is using the Chado Node: Title & Path API to allow custom
    // titles for your node type. Every time you want the title of the node, you
    // need to use the following API function:
    $node->title = chado_get_node_title($node);

  }
}

/**
 * Implementation of hook_node_presave().
 *
 * Performs actions on a node object prior to it being saved
 *
 * @ingroup tripal_onto_term_submit
 */
function tripal_onto_term_submit_node_presave($node) {
  // EXPLANATION: This node is useful for making changes to the node prior to it
  // being saved to the database.
  // One useful case for this is to set the title of a node using values
  // supplied by the user.
  //
  // This function is not required. You probably won't need it if you don't
  // define a custom node type in the hook_node_info() function. But it is node
  // type agnostic, so you can use this function to change the contents of any
  // node regardless of it's type.

  // set the node title
  switch ($node->type) {
    // This step is for setting the title for the Drupal node. This title is
    // permanent and thus is created to be unique. Title changes provided by
    // tokens are generated on the fly dynamically, but the node title seen in
    // the content listing needs to be set here. Do not call the
    // chado_get_node_title() function here to set the title as the node object
    // isn't properly filled out and the function will fail.
    case 'chado_onto_term_submit':
      // for a form submission the 'uniquename' field will be set,
      // for a sync, we must pull from the onto_term_submit object
      if (property_exists($node, 'term_name')) {
        // set the title
        //$library_term = tripal_get_cvterm(array('cvterm_id' => $node->onto_term_submit_id));
        $node->title = "Term Submission " . $node->created . ": " . $node->term_name  . ' by '.  $node->contact_name ;
     }
      else if (property_exists($node, 'onto_term_submit')) {
        $node->title = "Term Submission " . $node->created . " : " . $node->onto_term_submit->term_name  ." by ". $node->onto_term_submit->contact_name;
      }
      break;
  }
}

/**
 * Implementation of hook node_insert().
 *
 * Performs actions after any node has been inserted.
 *
 * @ingroup tripal_onto_term_submit
 */
function tripal_onto_term_submit_node_insert($node) {
  // EXPLANATION: This function is used after any a node is inserted into the
  // database. It is different from the hook_insert() function above in that it
  // is called after any node is saved, regardless of it's type. This function
  // is useful for making changes to the database after a node is inserted.
  // An onto_term_submit comes from the tripal_feature module where the URL alias of a
  // node cannot be set in the hook_insert() function. Therefore the
  // tripal_feature module uses this function to set the URL path of a newly
  // inserted onto_term_submit node.
  //
  // This function is not required. You probably won't need it if you don't
  // define a custom node type in the hook_node_info() function. But it is node
  // type agnostic, so you can use this function to do any activity after insert
  // of any node.

  // the Ontology Term Submission code below will set the URL path after inserting. We do it here
  // because we do not know the onto_term_submit_id in the pre-save and cannot do it in
  // the hook_insert()
  switch ($node->type) {
    case 'chado_onto_term_submit':

      // find the onto_term_submit and add in the details
      $onto_term_submit_id = chado_get_id_from_nid('onto_term_submit', $node->nid);
      // build the onto_term_submit variable by using the chado_generate_var() function
      $values = array('onto_term_submit_id' => $onto_term_submit_id);
      $onto_term_submit = chado_generate_var('onto_term_submit', $values);
      $node->onto_term_submit = $onto_term_submit;


      // EXPLANATION: You can allow the site admin to customize the
      // title and URL of your node.  The 'Chado Node: Title & Path API'
      // contains two functions that can be called to generate the title and
      // URL based a schema provided by the site admin. These functions are
      // named chado_get_node_title() and chado_set_node_url().  These
      // functions use a string of tokens to build the URL and titles and the
      // site admin has the ability to set these tokens.  There are
      // form elements made available in the tripal_onto_term_submit_admin() function
      // that allow the admin to set these tokens.  The default token string
      // is provided to Tripal using two hook functions, and are found below.
      // These are: chado_exmaple_chado_node_default_title() and
      // chado_onto_term_submit_chdo_node_default_url().

      // Set the Title and URL for this node.
      $onto_term_submit->title = chado_get_node_title($node);
      chado_set_node_url($node);
      break;
  }
}

/**
 * Implementation of hook node_update().
 *
 * Performs actions after any node has been updated.
 *
 */
function tripal_onto_term_submit_node_update($node) {
  // EXPLANATION: This function is used after any a node is updated in the
  // database. It is different from the hook_update() function above in that it
  // is called after any node is updated, regardless of it's type.
  // An onto_term_submit comes from the tripal_feature module where the URL alias of a
  // node cannot be set in the hook_update() function. Therefore the
  // tripal_feature module uses this function to reset the URL path of an
  // updated feature node.
  //
  // This function is not required. You probably won't need it if you don't
  // define a custom node type in the hook_node_info() function. But it is node
  // type agnostic, so you can use this function to do any activity after insert
  // of a node.

  // add items to other nodes, build index and search results
  switch ($node->type) {
    case 'chado_onto_term_submit':

      // If your module is using the Chado Node: Title & Path API to allow
      // custom titles for your node type. Every time you want the title of the
      // node, you need to use the following API function:
      $onto_term_submit->title = chado_get_node_title($node);

      // set the URL for this onto_term_submit page
      // see the code in the tripal_feature/includes/tripal_feature.chado_node.inc
      // file in the function tripal_feature_node_insert for an onto_term_submit of how
      // that module sets the URL. It uses a configuration file to allow the
      // user to dynamically build a URL schema and then uses that schema to
      // generate a URL string.
      break;
  }
}
/**
 * Implementation of hook_node_view().
 *
 * @ingroup tripal_onto_term_submit
 */
function tripal_onto_term_submit_node_view($node, $view_mode, $langcode) {
  // EXPLANATION: This function defines the content "blocks" that appear when
  // the node is displayed. It is node type agnostic so we can add content to
  // any node type. So, we use this function to add the content from all of our
  // theme templates onto our new node type. We will also use this function to
  // add content to other node types.

  switch ($node->type) {
    case 'chado_onto_term_submit':
      // there are different ways a node can be viewed. Primarily Tripal
      // supports full page view and teaser view.
      if ($view_mode == 'full') {

        // If you want to use the default Tripal node template then you need to
        // tell Tripal to generate the Table of Contents. This is done by
        // setting the following to TRUE. If your content type follows the
        // chado_<base table> convention then this is the default. In this case
        // if you don't want to use the default template then you need to set
        // the following to FALSE.
        $node->content['#tripal_generic_node_template'] = TRUE;

        // There is always a base template. This is the template that is first
        // shown when the onto_term_submit node type is first displayed.
        // If you are using the default Tripal node template, then you should
        // also set two additional items in each array:  tripal_toc_id and
        // tripal_toc_title. The tripal_tock_id should be a single unique
        // world that is used to reference the template. This ID is used for
        // constructing URLs for the content. The tripal_toc_title contains
        // the title that should appear in the table of contents for this
        // content. You should only set the '#weight' element for the base
        // template (or Overview) to ensure that it appears at the top of the
        // list. Otherwise items are sorted alphabetically.
        $node->content['tripal_onto_term_submit_base'] = array(
          '#theme' => 'tripal_onto_term_submit_base',
          '#node' => $node,
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
        // we can add other templates as well for properties, publications,
        // dbxrefs, etc...
        $node->content['tripal_onto_term_submit_properties'] = array(
          '#theme' => 'tripal_onto_term_submit_properties',
          '#node' => $node,
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
/*
        $node->content['tripal_onto_term_submit_references'] = array(
          '#theme' => 'tripal_onto_term_submit_references',
          '#node' => $node,
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
        $node->content['tripal_onto_term_submit_relationships'] = array(
          '#theme' => 'tripal_onto_term_submit_relationships',
          '#node' => $node,
          '#tripal_toc_id'    => 'relationships',
          '#tripal_toc_title' => 'Relationships',
        );
*/
        // Note: if you create a template that you do not want a user to know
        // where it is (discourage editing of it), you can add the following
        // key:  '#tripal_template_show' => FALSE. If this key/value is set the
        // administrator message that Tripal provides indicating where the
        // template is housed will not be shown.
      }
      // set the content for the teaser view
      if ($view_mode == 'teaser') {
        // The teaser is also a required template
        $node->content['tripal_onto_term_submit_teaser'] = array(
          '#theme' => 'tripal_onto_term_submit_teaser',
          '#node' => $node,
        );
      }
      break;
    // you can add custom content to any node type by adding content to the node
    // in the same way as above.
    case 'chado_organism':
      if ($view_mode == 'full') {
        $node->content['tripal_organism_onto_term_submits'] = array(
          '#theme' => 'tripal_organism_onto_term_submits',
          '#node' => $node,
          '#tripal_toc_id'    => 'onto_term_submits',
          '#tripal_toc_title' => 'Ontology Term Submissions',
        );
      }
      break;
    // ... etc
  }
}


/**
 * Implements [content_type]_chado_node_default_title_format().
 *
 * Defines a default title format for the Chado Node API to set the titles on
 * Chado onto_term_submit nodes based on chado fields.
 */
function chado_onto_term_submit_chado_node_default_title_format() {
  //return '[onto_term_submit.uniquename] ([onto_term_submit.type_id>cvterm.name]) [onto_term_submit.organism_id>organism.genus] [onto_term_submit.organism_id>organism.species]';
  return '[onto_term_submit.term_name] ([onto_term_submit.type_id>cv.name]) by [onto_term_submit.contact_name]';
}

/**
 * Implements hook_chado_node_default_url_format().
 *
 * Designates a default URL format for onto_term_submit nodes.
 */
function chado_onto_term_submit_chado_node_default_url_format() {
//  return '/onto_term_submit/[onto_term_submit.type_id>cv.name]/[onto_term_submit.term_name]';
  return '/onto_term_submit/[onto_term_submit.onto_term_submit_id]/[onto_term_submit.type_id>cv.name]/[onto_term_submit.term_name]';
}





function tripal_onto_term_submit_add_node_form_properties(&$form, &$form_state, $details) {
  // Set defaults for optional fields
  if (!array_key_exists('fieldset_title', $details)) {
    $details['fieldset_title'] = 'Properties';
  }
  if (!array_key_exists('additional_instructions', $details)) {
    $details['additional_instructions'] = '';
  }
  if (!array_key_exists('default_properties', $details)) {
    $details['default_properties'] = array();
  }
  if (!is_array($details['default_properties'])) {
    drupal_set_message("The 'default_properties' option must be an array", "error");
    tripal_report_error('tcprops_form', TRIPAL_ERROR, 
    "The 'default_properties' option must be an array", 
    array());
    return;
  }

  // make sure the property table exists before proceeding.
  if (!chado_table_exists($details['property_table'])) {
    drupal_set_message("Cannot add property elements to the form. The property table, '" .
      $details['property_table'] . "', does not exists", "error");
    tripal_report_error('tcprops_form', TRIPAL_ERROR, 
    "Cannot add property elements to the form. The property table, '%name', cannot be found.", 
    array('%name' => $details['property_table']));
    return;
  }

  // if the chado_id_field is not specified then set it using the
  // typical chado naming scheme
  if (!array_key_exists('chado_id_field', $details)) {
    $chado_id_table = preg_replace('/prop$/', '', $details['property_table']);
    $chado_id_field = $chado_id_table . '_id';
    $details['nodetype'] = $chado_id_table;
    $details['chado_id_field'] = $chado_id_field;
  }
  else {
    $details['nodetype'] = str_replace('_id', '', $details['chado_id_field']);
  }

  // make sure the specified cv exists
  if (isset($details['cv_name'])) {
    // make sure the cv_name is real
    $result = chado_select_record('cv', array('cv_id'), array('name' => $details['cv_name']));
    if (count($result) == 0) {
      drupal_set_message("Cannot add property elements to the form. The CV name, '" .
        $details['cv_name'] . "', does not exists", "error");
      tripal_report_error('tcprops_form', TRIPAL_ERROR, 
      "Cannot add property elements to the form. The CV named, '%name', cannot be found.", 
      array('%name' => $details['cv_name']));
      return;
    }
    // add the cv_id option to the details array
    $details['cv_id'] = $result[0]->cv_id;
  }
  elseif (isset($details['cv_id'])) {
    // make sure the cv_id is real
    $result = chado_select_record('cv', array('name'), array('cv_id' => $details['cv_id']));
    if (count($result) == 0) {
      drupal_set_message("Cannot add property elements to the form. The CV ID, '" .
        $details['cv_id'] . "', does not exist", "error");
      tripal_report_error('tcprops_form', TRIPAL_ERROR, 
      "Cannot add property elements to the form. The CV ID, '%id', cannot be found.", 
      array('%id' => $details['cv_id']));
      return;
    }
    // add the cv_name option to the details array
    $details['cv_name'] = $result[0]->name;
  }
 elseif( array_key_exists('select_options', $details)) {
   // check the contents in next block
 }
  
  
  else {

    // If we didn't get given a cv identifier, then try retrieving the default one
    // using the new cv defaults api
    $default_cv = tripal_get_default_cv($details['property_table'], 'type_id');
    if (!empty($default_cv)) {
      $details['cv_id'] = $default_cv->cv_id;
      $details['cv_name'] = $default_cv->name;
    }
    else {
      
      $default_form_link = l('vocabulary defaults configuration page', 
      'admin/tripal/chado/tripal_cv/defaults', 
      array('attributes' => array('target' => '_blank')));
      $message = "There is not a default vocabulary set for Property Types. Please set one using the $default_form_link.";

      if (preg_match('/(\w+)_id/', $details['chado_id_field'], $matches)) {
        $table = $matches[1];
        $table = ucwords(str_replace('_', ' ', $table));
        $message = "There is not a default vocabulary set for $table Property Types. Please set one using the  $default_form_link.";
      }

      tripal_set_message($message, TRIPAL_WARNING);
      tripal_report_error('tcprops_form', TRIPAL_ERROR, 
      "Please provide either a 'cv_name' or 'cv_id' as an option for adding properties to the form", 
      array());
    }
    return;
  }

  // Get property types for the select list. If the user has provided a set
  // then use those, otherwise get them from the cvterm table for specified cv.
  if (array_key_exists('select_options', $details) and 
    is_array($details['select_options'])) {
    $property_options = $details['select_options'];
  }
  // if the select options are not provided then try to get them on our own
  else {
    // if the vocabulary name is provided in the details then use that to
    // get the terms
    if (isset($details['cv_name'])) {
      $property_options = array();
      $property_options[] = 'Select a Property';
      $sql = "
        SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition, CV.cv_id as cv_id
        FROM  {cvterm} CVT
          INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
        WHERE
          CV.name = :cv_name AND
          NOT CVT.is_obsolete = 1
        ORDER BY CVT.name ASC
      ";
      $prop_types = chado_query($sql, array(':cv_name' => $details['cv_name']));
      while ($prop = $prop_types->fetchObject()) {
        $property_options[$prop->cvterm_id] = $prop->name;
      }
    }
    // if the cv_id is set in the $details array then use that to get the terms
    elseif (isset($details['cv_id'])) {
      $property_options = array();
      $property_options[] = 'Select a Property';
      $sql = "
        SELECT DISTINCT CVT.cvterm_id, CVT.name, CVT.definition, CV.name as cv_name
        FROM  {cvterm} CVT
          INNER JOIN {cv} CV ON CVT.cv_id = CV.cv_id
        WHERE
          CV.cv_id = :cv_id AND
          NOT CVT.is_obsolete = 1
        ORDER BY CVT.name ASC
      ";
      $prop_types = chado_query($sql, array(':cv_id' => $details['cv_id']));
      while ($prop = $prop_types->fetchObject()) {
        $property_options[$prop->cvterm_id] = $prop->name;
      }
    }
  }

  // Tell tripal administrators how to add terms to the property types drop down.
  if (empty($property_options)) {
    $tripal_message = tripal_set_message(
    t('There are currently no property types! To add properties to the drop
        down list, you need to <a href="@cvtermlink">add a controlled vocabulary term</a>
        to the %cv_name controlled vocabulary.', 
    array(
      '%cv_name' => $details['cv_name'],
      '@cvtermlink' => url('admin/tripal/chado/tripal_cv/cv/' . $details['cv_id'] . '/cvterm/add')
    )
    ), 
    TRIPAL_NOTICE, 
    array('return_html' => TRUE)
    );
  }
  else {
//    $tripal_message = tripal_set_message(
//    t('To add additional properties to the drop down list, you need to <a href="@cvtermlink">add
//        a controlled vocabulary term</a> to the %cv_name controlled vocabulary.', 
//    array(
//      '%cv_name' => $details['cv_name'],
//      '@cvtermlink' => url('admin/tripal/chado/tripal_cv/cv/' . $details['cv_id'] . '/cvterm/add')
//    )
//    ), 
//    TRIPAL_INFO, 
//    array('return_html' => TRUE)
 //   );
  }
  // Group all of the chado node api fieldsets into vertical tabs.
  $form['chado_node_api'] = array(
    '#type' => 'vertical_tabs',
    '#attached' => array(
      'css' => array(
        'chado-node-api' => drupal_get_path('module', 'tripal_core') . '/theme/css/chado_node_api.css',
      ),
    ),
  );

  // the fieldset of the property elements
  $instructions = 'To add relationships to the new term, select the relationship type 
     from the first drop-down below and then select the relation from the second drop-down before 
      clicking "Add". To remove incorrect information, click the "Remove" button. 
      Note: you cannot edit previously added information but instead need to 
      remove and re-add it. If you would like to use a relationship or relation term that is not listed, add them to the Comments field above.';
  $form['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t($details['fieldset_title']),
    '#description' => t('<p><strong>Add Relationsips</strong></p><p>' . $instructions . $details['additional_instructions'] . '</p>', array('%nodetype' => $details['nodetype'])),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'chado_node_api',
    '#weight' => 8,
    '#attributes' => array('class' => array('chado-node-api', 'properties')),
    '#attached' => array(
      'js' => array(
        'chado-node-api-vertical-tabs' => drupal_get_path('module', 'tripal_core') . '/theme/js/chadoNodeApi_updateVerticalTabSummary.js',
      ),
    ),
  );

  // this form element is a tree, so that we don't puke all of the values into then node variable
  // it is set as a tree, and keeps them in the $form_state['values']['property_table'] heading.
  $form['properties']['property_table'] = array(
    '#type' => 'markup',
    '#tree' => TRUE,
    '#prefix' => '<div id="tripal-generic-edit-properties-table">',
    '#suffix' => '</div>',
    '#theme' => 'chado_node_properties_form_table'
  );

  // We need to provide feedback to the user that changes made
  // are not saved until the node is saved.
  $form['properties']['property_table']['save_warning'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="property-save-warning" class="messages warning" style="display:none;">',
    '#suffix' => '</div>',
    '#markup' => '* The changes to these properties will not be saved until the 
      "Save" button at the bottom of this form is clicked. <span class="specific-changes"></span>',
    '#attached' => array(
      'js' => array(
        'chado-node-api-unsaved' => drupal_get_path('module', 'tripal_core') . '/theme/js/chadoNodeApi_unsavedNotify.js',
      ),
    ),
  );

  // Add defaults into form_state to be used elsewhere
  $form['properties']['property_table']['details'] = array(
    '#type' => 'hidden',
    '#value' => serialize($details)
  );

  /* Properties can come to us in two ways:
   * 1) As entries in the $details['default_properties'] option
   *
   * 2) In the form state in the $form_state['chado_properties']. Data is in this field
   *    when an AJAX call updates the form state or a validation error.
   *
   * 3) Directly from the database if the record already has properties associated.  This
   *    data is only used the first time the form is loaded. On AJAX calls or validation
   *    errors the fields on the form are populated from the $form_state['chado_properties']
   *    entry.
   */
  if (isset($form_state['chado_properties'])) {
    $existing_properties = $form_state['chado_properties'];
  }
  else {

    // build the SQL for extracting properties already assigned to this record
    $sql_args = array();
    $sql_args[':chado_id'] = $details['chado_id'];
    // NOT USING DEFAULT or ASSIGNED CV in tripal_onto_term_submit
   /*
    if (array_key_exists('cv_name', $details)) {
      $cv_where = "AND CV.name = :cvname";
      $sql_args[':cvname'] = $details['cv_name'];
    }
    elseif (array_key_exists('cv_id', $details)) {
      $cv_where = "AND CV.cv_id = :cvid";
      $sql_args[':cvid'] = $details['cv_id'];
    }
   */
    $existing_properties = chado_query(
    "SELECT
         PP." . $details['property_table'] . "_id property_id,
         CVT.cvterm_id as type_id,
         CVT.name as type_name,
         CVT.definition,
         CVT2.name as parent_name,
         CVT2.definition as parent_definition,
         PP.value ,
         PP.rank
       FROM {" . $details['property_table'] . "} PP
         INNER JOIN {cvterm} CVT ON CVT.cvterm_id = PP.type_id
         INNER JOIN {cvterm} CVT2 ON CVT2.cvterm_id = PP.value
       WHERE
         PP." . $details['chado_id_field'] . " = :chado_id 
       ORDER BY CVT.name, PP.rank", $sql_args)->fetchAll();

    // next add in any default properties
    if (array_key_exists('default_properties', $details)) {

      // next iterate through each of the default properties and create a new
      // stdClass array that contains the fields needed.
      foreach ($details['default_properties'] as $property) {
        $new_prop = new stdClass();
        $new_prop->type_id = $property['cvterm']->cvterm_id;
        $new_prop->type_name = $property['cvterm']->name;
        $new_prop->definition = $property['cvterm']->definition;
        //$new_prop->parent_definition = $property['cvterm']->definition;
        $new_prop->value = $property['value'];
        $new_prop->property_id = 'TEMP' . uniqid();
        $new_prop->rank = 'TEMP' . uniqid();
        $existing_properties[] = $new_prop;
      }
    }
  }
  /* The format of the $existing_properties array is either:
   *
   * From the chado_properties array:
   * $form_state['chado_properties'] = array(
   *   '[type_id]-[rank]' => array(
   *     'type_id' => [the cvterm.cvterm_id value]
   *     'type_name' => [the cvterm.name value]
   *     'property_id' => [the property.property_id value, or temporary value if it doesn't yet exist],
   *     'parent_type_id' => [the BASEprop.value value],  // was 'value' => 
   *     'parent_name' => [the BASEprop.value value],  
   *     'parent_definition' => [the BASEprop.value value],  
   *     'rank' => [the BASEprop.rank value or NULL if not saved yet],
   *   ),
   * );
   *
   * OR
   * Populated from the database:
   * $existing_property = array(
   *   0 => array(
   *     'property_id' => [the property.property_id value],
   *     'type_id' => [the cvterm.cvterm_id value]
   *     'type_name' => [the cvterm.name value]
   * //    'value' => [the BASEprop.value value],
   *     'parent_type_id' => [the BASEprop.value value],  // was 'value' => 
   *     'parent_name' => [the BASEprop.value value],  
   *     'parent_definition' => [the BASEprop.value value],  
   *     'rank' => [the BASEprop.rank value],
   *   ),
   * );
   *
   * NOTE: The main difference is the key
   *
   * Loop on the array elements of the $existing_properties array and add
   * an element to the form for each one as long as it's also in the
   * $properties_options array.
   */
  $num_properties = 0;
  foreach ($existing_properties as $property) {
    if (array_key_exists($property->type_id, $property_options)) {
      $num_properties++;

      $form['properties']['property_table'][$property->type_id]['#type'] = 'markup';
      $form['properties']['property_table'][$property->type_id]['#value'] = 'markup';

      $form['properties']['property_table'][$property->type_id][$property->property_id]['#type'] = 'markup';
      $form['properties']['property_table'][$property->type_id][$property->property_id]['#value'] = 'markup';
      $form['properties']['property_table'][$property->type_id][$property->property_id]['#attributes'] = array(
        'class' => array('property', 'saved')
      );

      // Determine whether this property is unsaved or not.
      // We can tell this by looking at the property_id: if it's not
      // saved yet we will have entered a TEMP###.
      if (preg_match('/^TEMP/', $property->property_id)) {
        $form['properties']['property_table'][$property->type_id][$property->property_id]['#attributes'] = array(
          'class' => array('property', 'unsaved')
        );
      }

      $form['properties']['property_table'][$property->type_id][$property->property_id]['prop_type_id'] = array(
        '#type' => 'hidden',
        '#value' => $property->type_id
      );

      $form['properties']['property_table'][$property->type_id][$property->property_id]['prop_value'] = array(
        '#type' => 'hidden',
        '#value' => $property->value
      );

      $form['properties']['property_table'][$property->type_id][$property->property_id]['prop_rank'] = array(
        '#type' => 'hidden',
        '#value' => $property->rank
      );

      $form['properties']['property_table'][$property->type_id][$property->property_id]['property_id'] = array(
        '#type' => 'hidden',
        '#value' => $property->property_id
      );

      $form['properties']['property_table'][$property->type_id][$property->property_id]['type'] = array(
        '#type' => 'markup',
        '#markup' => $property->type_id,
        '#prefix' => '<span class="row-unsaved-warning"></span>'
      );
      // If a definition is available we want to add that to the type column
      // to make it easier for users to determine what an added property means.
      if (isset($property->definition)) {
        $form['properties']['property_table'][$property->type_id][$property->property_id]['type']['#markup'] = $property->type_name . '<br><i>' . $property->definition . '</i>';
      }else{
        $new_term = tripal_get_cvterm(array('cvterm_id' => $property->type_id));
        if ($new_term) {
      //     $property->definition = $new_term->definition;
            if (isset($new_term->definition)) {
                $form['properties']['property_table'][$property->type_id][$property->property_id]['type']['#markup'] = $new_term->name . '<br><i>' . $new_term->definition . '</i>';
         }

      }
     }


      $form['properties']['property_table'][$property->type_id][$property->property_id]['value'] = array(
        '#type' => 'markup',
        '#markup' => $property->value ,
        '#value' => $property->value ,
        '#prefix' => '<span class="row-unsaved-warning"></span>',
      );
    $new_term = tripal_get_cvterm(array('cvterm_id' => $property->value));
    if ($new_term) {
      $property->parent_definition = $new_term->definition;
      $property->parent_name = $new_term->name;
    }

      if (isset($property->parent_definition)) {
        $form['properties']['property_table'][$property->type_id][$property->property_id]['value']['#markup'] = $property->parent_name . '<br><i>' . $property->parent_definition . '</i>';
      }else{
         $new_term = tripal_get_cvterm(array('cvterm_id' => $property->value));
          if ($new_term) {
            $property->parent_definition = $new_term->definition;
            $property->parent_name = $new_term->name;
          }
         if (isset($new_term->definition)) {
        $form['properties']['property_table'][$property->type_id][$property->property_id]['value']['#markup'] = $new_term->name . '<br><i>' . $new_term->definition . '</i>';
     }

}

      $form['properties']['property_table'][$property->type_id][$property->property_id]['rank'] = array(
        '#type' => 'markup',
        '#markup' => $property->rank
      );
      // remove button
      $form['properties']['property_table'][$property->type_id][$property->property_id]['property_action'] = array(
        '#type' => 'submit',
        '#value' => t('Remove'),
        '#name' => "properties_remove-" . $property->type_id . '-' . $property->property_id,
        '#ajax' => array(
          'callback' => "chado_add_node_form_subtable_ajax_update",
          'wrapper' => 'tripal-generic-edit-properties-table',
          'effect' => 'fade',
          'method' => 'replace',
          'prevent' => 'click'
        ),
        // When this button is clicked, the form will be validated and submitted.
        // Therefore, we set custom submit and validate functions to override the
        // default node form submit.  In the validate function we validate only the
        // property fields and in the submit we remove the indicated property
        // from the chado_properties array. In order to keep validate errors
        // from the node form validate and Drupal required errors for non-property fields
        // preventing the user from removing properties we set the #limit_validation_errors below
        '#validate' => array('chado_add_node_form_subtables_remove_button_validate'),
        '#submit' => array('tripal_onto_term_submit_add_node_form_subtables_remove_button_submit'),
        // Limit the validation of the form upon clicking this button to the property_table tree
        // No other fields will be validated (ie: no fields from the main form or any other api
        // added form).
        '#limit_validation_errors' => array(
          array('property_table') // Validate all fields within $form_state['values']['property_table']
        ),
      );
    }
  }

  // Quickly add a hidden field stating how many properties are currently added.
  $form['properties']['num_properties'] = array(
    '#type' => 'hidden',
    '#value' => $num_properties,
    '#attributes' => array('class' => 'num-properties')
  );

  // Form elements for adding a new property
  //---------------------------------------------
  $form['properties']['property_table']['new'] = array(
    '#type' => 'markup',
    '#prefix' => '<span class="addtl-properties-add-new-property">',
    '#suffix' => '</span>'
  );

  // get the value selected (only works on AJAX call) and print the
  // description
  $type_desc = '';
  if (isset($form_state['input']['property_table']['new']['type'])) {
    $new_type_id = $form_state['input']['property_table']['new']['type'];
    $new_term = tripal_get_cvterm(array('cvterm_id' => $new_type_id));
    if ($new_term) {
      $type_desc = $new_term->definition;
    }
  }
  $parent_desc = '';
  if (isset($form_state['input']['property_table']['new']['value'])) {
    $new_type_id = $form_state['input']['property_table']['new']['value'];
    $new_term = tripal_get_cvterm(array('cvterm_id' => $new_type_id));
    if ($new_term) {
      $parent_desc = $new_term->definition;
    }
  }
  $form['properties']['property_table']['new']['type'] = array(
    '#type' => 'select',
    '#options' => $property_options, // Set at top of form
    '#prefix' => '<span id="tripal-generic-edit-properties-new-desc">',
    '#suffix' => '<i>' . $type_desc . '</i></span>',
    '#ajax' => array(
      'callback' => "chado_add_node_form_properties_ajax_desc",
      'wrapper' => 'tripal-generic-edit-properties-new-desc',
      'effect' => 'fade',
      'method' => 'replace',
    ),
  );



  $form['properties']['property_table']['new']['value'] = array(
    '#type' => 'select',
    '#options' => $details['value_options'],
    '#prefix' => '<span id="tripal-generic-edit-properties-new-parent-desc">',
    '#suffix' => '<i>' . $parent_desc . '</i></span>',
    '#ajax' => array(
      'callback' => "chado_add_node_form_properties_ajax_parent_desc",
      'wrapper' => 'tripal-generic-edit-properties-new-parent-desc',
      'effect' => 'fade',
      'method' => 'replace',
    ),
  );
  // add button
  $form['properties']['property_table']['new']['property_action'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
    '#name' => "properties-add",
    '#ajax' => array(
      'callback' => "chado_add_node_form_subtable_ajax_update",
      'wrapper' => 'tripal-generic-edit-properties-table',
      'effect' => 'fade',
      'method' => 'replace',
      'prevent' => 'click'
    ),
    // When this button is clicked, the form will be validated and submitted.
    // Therefore, we set custom submit and validate functions to override the
    // default node form submit.  In the vali date function we validate only the
    // additional property fields and in the submit we add them to the chado_properties
    // array. In order to keep validate errors from the node form validate and Drupal
    // required errors for non-property fields preventing the user from adding properties we
    // set the #limit_validation_errors below
    '#validate' => array('chado_add_node_form_subtables_add_button_validate'),
    '#submit' => array('tripal_onto_term_submit_add_node_form_subtables_add_button_submit'),
    // Limit the validation of the form upon clicking this button to the property_table tree
    // No other fields will be validated (ie: no fields from the main form or any other api
    // added form).
    '#limit_validation_errors' => array(
      array('property_table') // Validate all fields within $form_state['values']['property_table']
    )
  );
$tripal_message = "some message";
  $form['properties']['admin_message'] = array(
    '#type' => 'markup',
    '#markup' => $tripal_message
  );
}


function chado_add_node_form_properties_ajax_parent_desc($form, $form_state) {
  return $form['properties']['property_table']['new']['value'];
}


function tripal_onto_term_submit_add_node_form_subtables_add_button_submit($form, &$form_state) {

  // Based on triggering element call the correct submit function
  // ASUMPTION #1: each of the buttons must have properties, dbxrefs or relationships
  // as the first part of the #name to uniquely identify the subsection.
  if (preg_match('/^([a-z]+).*/', $form_state['triggering_element']['#name'], $matches)) {
    $subsection = $matches[1];

    switch ($subsection) {
      case 'properties':
        tripal_onto_term_submit_add_node_form_properties_add_button_submit($form, $form_state);
        break;
      case 'dbxrefs':
        chado_add_node_form_dbxrefs_add_button_submit($form, $form_state);
        break;
      case 'relationships':
        chado_add_node_form_relationships_add_button_submit($form, $form_state);
        break;
    }
  }

  // This is needed to ensure the form builder function is called for the node
  // form in order for any of these changes to be seen.
  $form_state['rebuild'] = TRUE;
}

function tripal_onto_term_submit_add_node_form_properties_add_button_submit($form, &$form_state) {

  $details = unserialize($form_state['values']['property_table']['details']);

  // if the chado_additional_properties array is not set then this is the first time modifying the
  // property table. this means we need to include all the properties from the db
  if (!isset($form_state['chado_properties'])) {
    tripal_onto_term_submit_add_node_form_properties_create_property_formstate_array($form, $form_state);
  }

  // get details for the new property
  $property = array(
    'type_id' => $form_state['values']['property_table']['new']['type'],
    'type_name' => $form_state['values']['property_table']['new']['type_name'],
    'definition' => $form_state['values']['property_table']['new']['definition'],
    'property_id' => 'TEMP' . uniqid(),
    'value' => $form_state['values']['property_table']['new']['value'],
    'rank' => 'TEMP' . uniqid(),
  );

  $key = $property['type_id'] . '-' . $property['property_id'];
  $form_state['chado_properties'][$key] = (object) $property;

  // we don't want the new element to pick up the values from the previous element so wipe them out
  unset($form_state['input']['property_table']['new']['type']);
  unset($form_state['input']['property_table']['new']['type_name']);
  unset($form_state['input']['property_table']['new']['definition']);
  unset($form_state['input']['property_table']['new']['value']);

}
function tripal_onto_term_submit_add_node_form_properties_create_property_formstate_array($form, &$form_state) {

  $form_state['chado_properties'] = array();

  foreach (element_children($form['properties']['property_table']) as $type_id) {
    if ($type_id != 'new') {
      foreach (element_children($form['properties']['property_table'][$type_id]) as $property_id) {
        $element = $form['properties']['property_table'][$type_id][$property_id];
        $property = array(
          'type_id' => $element['prop_type_id']['#value'],
          'type_name' => $element['type']['#markup'],
          'property_id' => $element['property_id']['#value'],
          'value' => $element['value']['#value'],
          'rank' => $element['rank']['#markup']
        );
        $key = $property['type_id'] . '-' . $property['property_id'];
        $form_state['chado_properties'][$key] = (object) $property;
      }
    }
  }
}
function tripal_onto_term_submit_add_node_form_subtables_remove_button_submit($form, &$form_state) {

  // Based on triggering element call the correct submit function
  // ASUMPTION #1: each of the buttons must have properties, dbxrefs or relationships
  // as the first part of the #name to uniquely identify the subsection.
  if (preg_match('/^([a-z]+).*/', $form_state['triggering_element']['#name'], $matches)) {
    $subsection = $matches[1];

    switch ($subsection) {
      case 'properties':
        tripal_onto_term_submit_add_node_form_properties_remove_button_submit($form, $form_state);
        break;
      case 'dbxrefs':
        chado_add_node_form_dbxrefs_remove_button_submit($form, $form_state);
        break;
      case 'relationships':
        chado_add_node_form_relationships_remove_button_submit($form, $form_state);
        break;
    }
  }

  // This is needed to ensure the form builder function is called for the node
  // form in order for any of these changes to be seen.
  $form_state['rebuild'] = TRUE;
}
function tripal_onto_term_submit_add_node_form_properties_remove_button_submit(&$form, &$form_state) {

  // if the chado_properties array is not set then this is the first time modifying the
  // property table. this means we need to include all the properties from the db
  if (!isset($form_state['chado_properties'])) {
    tripal_onto_term_submit_add_node_form_properties_create_property_formstate_array($form, $form_state);
  }

  // remove the specified property from the form property table
  if (preg_match('/properties_remove-([^-]+-[^-]+)/', $form_state['triggering_element']['#name'], $match)) {
    $key = $match[1];
    if (array_key_exists($key, $form_state['chado_properties'])) {
      unset($form_state['chado_properties'][$key]);
    }
  }
}
