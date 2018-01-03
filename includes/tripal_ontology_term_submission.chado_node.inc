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
 * @ingroup tripal_ontology_term_submission
 */
function tripal_ontology_term_submission_node_info() {
  $nodes = array();

  // EXPLANATION: this array describes all of the node types that are created
  // by this module. For many Tripal modules (e.g. tripal_ontology_term_submission, tripal_stock,
  // tripal_library, tripal_pub, etc.) new node types are created. It is
  // customary to name all new node types that interact with data in Chado
  // with a 'chado_' prefix.

  $nodes['chado_ontology_term_submission'] = array(
    'name'        => t('Ontology Term Submission'),
    'base'        => 'chado_ontology_term_submission',
    'description' => t('A record from the fake chado ontology_term_submission table'),
    'has_title'   => TRUE,
    'locked'      => TRUE,
    // EXPLANATION: This section of the node type array specifies how Tripal
    // will sync the node types with data in Chado. When Drupal creates a node
    // it has no way of coordinating which node belongs to which record in
    // Chado. Therefore, Tripal maintains tables in the Drupal schema that maps
    // Drupal nodes to records in Chado. Syncing is the process of creating
    // Drupal nodes and linking them to the appropriate record.
    'chado_node_api' => array(
      // the base table name (e.g. ontology_term_submission, ontology_term_submission, contact)
      'base_table' => 'ontology_term_submission',
      // the node type hook prefix
      'hook_prefix' => 'chado_ontology_term_submission',
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
 * available permissions are set in the chado_ontology_term_submission_permissions() hook in the
 * tripal_ontology_term_submission.module file. This hook is not needed if no node types were
 * defined in the hook_node_info() hook.
 *
 * @return
 * This function should return null if it does not specifically deny access.
 * This allows for other mechanisms to to deny or reject access. If the return
 * value is TRUE then access is granted regardless of any other rules that might
 * be implemented by other modules.
 */
function tripal_ontology_term_submission_node_access($node, $op, $account) {
  $node_type = $node;
  if (is_object($node)) {
    $node_type = $node->type;
  }
  // EXPLANATION:  in the tripal_ontology_term_submission_permissions() function we created the
  // permission types that are used here to check for access permissions to the
  // 'chado_exmaple' node type.
  if($node_type == 'chado_ontology_term_submission') {
    if ($op == 'create') {
      if (!user_access('create chado_ontology_term_submission content', $account)) {
        return NODE_ACCESS_DENY;
      }
      return NODE_ACCESS_ALLOW;
    }
    if ($op == 'update') {
      if (!user_access('edit chado_ontology_term_submission content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'delete') {
      if (!user_access('delete chado_ontology_term_submission content', $account)) {
        return NODE_ACCESS_DENY;
      }
    }
    if ($op == 'view') {
      if (!user_access('access chado_ontology_term_submission content', $account)) {
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
 * @ingroup tripal_ontology_term_submission
 */
function chado_ontology_term_submission_form($node, &$form_state) {

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
  // For the ontology_term_submission code below we assume that the fake 'ontology_term_submission' table only has
  // a uniquename, organism_id, type_id and ontology_term_submission_id.

  $form = array();

  // Default values can come in the following ways:
  //
  // 1) as elements of the $node object. This occurs when editing an existing
  //    ontology_term_submission
  // 2) in the $form_state['values'] array which occurs on a failed validation
  //    or ajax callbacks from non submit form elements
  // 3) in the $form_state['input'[ array which occurs on ajax callbacks from
  //    submit form elements and the form is being rebuilt
  //
  // set form field defaults


  // SET FORM DEFAULTS
  //---------------------------------------------
  $ontology_term_submission      = null; // holds the ontology_term_submission object record
  $ontology_term_submission_id   = null; // when editing an ontology_term_submission record we'll have an ontology_term_submission_id

  // initialize the defaults for the form fields
  $name   = '';
  $definition   = '';
  $definition_ref   = '';
  $term_xref   = '';
  $ontology_term_submission_type = '';
  $seeAlso_url  = '';
  $contact_name  = '';
  $contact_email = '';
  $comments = '';
  $parent_cvterm_id_1 = '';
  $parent_cvterm_id_2 = '';
  $parent_cvterm_id_3 = '';
  $relation_cvterm_id_1 = '';
  $relation_cvterm_id_2 = '';
  $relation_cvterm_id_3 = '';

  // if we are editing an existing node then the 'ontology_term_submission' record from Chado
  // is already part of the node, so we set the defaults from that object
  if (property_exists($node, 'ontology_term_submission')) {
    $ontology_term_submission = $node->ontology_term_submission;
    $ontology_term_submission_id   = $ontology_term_submission->ontology_term_submission_id;
    $name   = $ontology_term_submission->name;
    $definition  = $ontology_term_submission->definition;
    $definition_ref  = $ontology_term_submission->definition_ref;
    $term_xref  = $ontology_term_submission->term_xref;
    $ontology_term_submission_type = $ontology_term_submission->ontology_term_submission_type;
  $seeAlso_url  = $ontology_term_submission->seeAlso_url;
  $contact_name  = $ontology_term_submission->contact_name;
  $contact_email = $ontology_term_submission->contact_email;
  $comments = $ontology_term_submission->comments;
  $parent_cvterm_id_1 = $ontology_term_submission->parent_cvterm_id_1;
  $parent_cvterm_id_2 = $ontology_term_submission->parent_cvterm_id_2;
  $parent_cvterm_id_3 = $ontology_term_submission->parent_cvterm_id_3;
  $relation_cvterm_id_1 = $ontology_term_submission->relation_cvterm_id_1;
  $relation_cvterm_id_2 = $ontology_term_submission->relation_cvterm_id_2;
  $relation_cvterm_id_3 = $ontology_term_submission->relation_cvterm_id_3;

    // keep track of the ontology_term_submission id
    $form['ontology_term_submission_id'] = array(
      '#type' => 'value',
      '#value' => $ontology_term_submission_id,
    );
  }
  // if we are re constructing the form from a failed validation or ajax
  // callback then use the $form_state['values'] values
  if (array_key_exists('values', $form_state)) {
     $name   = $form_state['values']['name'];
     $definition  = $form_state['values']['definition'];
     $definition_ref  = $form_state['values']['definition_ref'];
     $ontology_term_submission_type = $form_state['values']['ontology_term_submission_type'];
     $seeAlso_url  = $form_state['values']['seeAlso_url'];
     $contact_name  = $form_state['values']['contact_name'];
     $contact_email  = $form_state['values']['contact_email'];
     $comments  = $form_state['values']['comments'];
     $parent_cvterm_id_1 = $form_state['values']['parent_cvterm_id_1'];
     $parent_cvterm_id2 = $form_state['values']['parent_cvterm_id_2'];
     $parent_cvterm_id_3 = $form_state['values']['parent_cvterm_id_3'];
     $relation_cvterm_id_1 = $form_state['values']['relation_cvterm_id_1'];
     $relation_cvterm_id_2 = $form_state['values']['relation_cvterm_id_2'];
     $relation_cvterm_id_3 = $form_state['values']['relation_cvterm_id_3'];
  }
  // if we are re building the form from after submission (from ajax call) then
  // the values are in the $form_state['input'] array
  if (array_key_exists('input', $form_state) and !empty($form_state['input'])) {
     $name   = $form_state['input']['name'];
     $definition  = $form_state['input']['definition'];
     $definition_ref  = $form_state['input']['definition_ref'];
     $ontology_term_submission_type = $form_state['input']['ontology_term_submission_type'];
     $seeAlso_url  = $form_state['input']['seeAlso_url'];
     $contact_name  = $form_state['input']['contact_name'];
     $contact_email  = $form_state['input']['contact_email'];
     $comments  = $form_state['input']['comments'];
     $parent_cvterm_id_1 = $form_state['input']['parent_cvterm_id_1'];
     $parent_cvterm_id2 = $form_state['input']['parent_cvterm_id_2'];
     $parent_cvterm_id_3 = $form_state['input']['parent_cvterm_id_3'];
     $relation_cvterm_id_1 = $form_state['input']['relation_cvterm_id_1'];
     $relation_cvterm_id_2 = $form_state['input']['relation_cvterm_id_2'];
     $relation_cvterm_id_3 = $form_state['input']['relation_cvterm_id_3'];
  }


  // FORM ELEMENTS
  //---------------------------------------------
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Term Name'),
    '#required' => TRUE,
    '#default_value' => $name,
    '#description' => t('Enter a new term name.'),
    '#maxlength' => 255
  );

  // for the type_id we want to use the default vocabulary so that this field
  // can have auto-complete functionality
  $cv_id = $ontology_term_submission_type->cv_id;
     $sql = "SELECT distinct cv.cv_id , cv.name
         FROM {cv} cv
         WHERE name in ('Planarian_Anatomy' , 'Schmidtea_mediterranea_Developmental_Terms' , 'Schmidtea_mediterranea_Developmental_Stages')
         ORDER BY cv.name";

    $results = chado_query($sql,array(':cv_id' => $cv_id);
    $libraries[] = '';
    foreach ($results as $library) {
      $libraries[$library->cv_id] = $library->name;
    }
  $form['ontology_term_submission_type'] = array(
    '#title'       => t('Ontology Term Submission Type'),
    '#type'        => t('select'),
    '#description' => t("Choose the ontology_term_submission type (e.g. Test Type)."),
    '#required'    => TRUE,
    '#options' => $libraries,
    '#selected' => $cv_id
  );

  $form['definition'] = array(
    '#type' => 'text_format',
    '#title' => t('Definition'),
    '#required' => TRUE,
    '#default_value' => $definition,
    '#description' => t('Enter a def for this ontology_term_submission.'),
 
  $form['definition_ref'] = array(
    '#type' => 'text_format',
    '#title' => t('Definition Reference'),
    '#required' => TRUE,
    '#default_value' => $definition_ref,
    '#description' => t('Enter a def reference for this ontology_term_submission.'),
  ); 

  $form['seeAlso_url'] = array(
    '#type' => 'text_format',
    '#title' => t('seeAlso URL'),
    '#default_value' => $seeAlso_url,
    '#description' => t('Enter a URL for more rich info for this ontology_term_submission.'),
  );

  $form['contact_name'] = array(
    '#type' => 'text_format',
    '#title' => t('Your Name'),
    '#required' => TRUE,
    '#default_value' => $contact_name,
    '#description' => t('Enter your name for this ontology_term_submission.'),
  );
  $form['contact_email'] = array(
    '#type' => 'text_format',
    '#title' => t('Your E-mail address'),
    '#required' => TRUE,
    '#default_value' => $contact_email,
    '#description' => t('Enter your email address for us to contact you about this ontology_term_submission.'),

  $form['comments'] = array(
    '#type' => 'text_format',
    '#title' => t('Any Comments'),
    '#default_value' => $comments,
    '#description' => t('Enter any comments about this ontology_term_submission.'),

  );
     $sql = "SELECT distinct cvt.cvterm_id , cvt.name
         FROM {cvterm} cvt, {cv} cv
         WHERE cv.name in ('Planarian_Anatomy' , 'Schmidtea_mediterranea_Developmental_Terms' , 'Schmidtea_mediterranea_Developmental_Stages')
           AND cvt.cv_id = cv.cv_id
         ORDER BY cvt.name";
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

  $form['relation_cvterm_id_1'] = array(
    '#title'       => t('Relation 1'),
    '#type'        => t('select'),
    '#description' => t("Choose the Relation Term  (e.g. is_a)."),
    '#required'    => TRUE,
    '#options' => $all_relations,
  );
  $form['parent_cvterm_id_1'] = array(
    '#title'       => t('Parent 1'),
    '#type'        => t('select'),
    '#description' => t("Choose the Parent Term  (e.g. neuron)."),
    '#required'    => TRUE,
    '#options' => $all_terms,
  );
  $form['relation_cvterm_id_2'] = array(
    '#title'       => t('Relation 2'),
    '#type'        => t('select'),
    '#description' => t("Choose the Relation Term  (e.g. is_a)."),
    '#required'    => TRUE,
    '#options' => $all_relations,
  );
  $form['parent_cvterm_id_2'] = array(
    '#title'       => t('Parent 2'),
    '#type'        => t('select'),
    '#description' => t("Choose the Parent Term  (e.g. neuron)."),
    '#required'    => TRUE,
    '#options' => $all_terms,
  );
  $form['relation_cvterm_id_3'] = array(
    '#title'       => t('Relation 3'),
    '#type'        => t('select'),
    '#description' => t("Choose the Relation Term  (e.g. is_a)."),
    '#required'    => TRUE,
    '#options' => $all_relations,
  );
  $form['parent_cvterm_id_3'] = array(
    '#title'       => t('Parent 3'),
    '#type'        => t('select'),
    '#description' => t("Choose the Parent Term  (e.g. neuron)."),
    '#required'    => TRUE,
    '#options' => $all_terms,
  );



);

  // PROPERTIES FORM
  //---------------------------------------------
  // If there is a ontology_term_submissionprop table and you want to allow users to add/remove
  // entries from it through your node form then add this section to your own
  // node form
/*
  $prop_cv = tripal_get_default_cv('ontology_term_submissionprop', 'type_id');
  $cv_id = $prop_cv ? $prop_cv->cv_id : NULL;
  $details = array(
    // the name of the prop table
    'property_table' => 'ontology_term_submissionprop',
    // the value of ontology_term_submission_id for this record
    'chado_id' => $ontology_term_submission_id,
    // the cv.cv_id of the cv governing ontology_term_submissionprop.type_id
    'cv_id' => $cv_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_properties($form, $form_state, $details);

  // ADDITIONAL DBXREFS FORM
  //---------------------------------------------
  // If there is a ontology_term_submission_dbxref table and you want to allow users to
  // add/remove entries from it through your node form then add this section to
  // your own node form
  $details = array(
    // the name of the _dbxref table
    'linking_table' => 'ontology_term_submission_dbxref',
    // the name of the key in your base chado table
    'base_foreign_key' => 'ontology_term_submission_id',
    // the value of ontology_term_submission_id for this record
    'base_key_value' => $ontology_term_submission_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_dbxrefs($form, $form_state, $details);
*/
  // RELATIONSHIPS FORM
  //---------------------------------------------
  // If there is a ontology_term_submission_relationship table and you want to allow users to
  // add/remove entries from it through your node form then add this section to
  // your own node form
/*
  $rels_cv = tripal_get_default_cv('ontology_term_submission_relationship', 'type_id');
  $cv_id = $rels_cv ? $rels_cv->cv_id : NULL;
  $details = array(
    // the name of the _relationship table
    'relationship_table' => 'ontology_term_submission_relationship',
    // the name of your chado base table
    'base_table' => 'ontology_term_submission',
    // the name of the key in your base chado table
    'base_foreign_key' => 'ontology_term_submission_id',
    // the value of ontology_term_submission_id for this record
    'base_key_value' => $ontology_term_submission_id,
    // the human-readable name of your node type
    'nodetype' => 'ontology_term_submission',
    // the cv.cv_id of the cv governing ontology_term_submission_relationship.type_id
    'cv_id' => $cv_id
  );
  // Adds the form elements to your current form
  chado_add_node_form_relationships($form, $form_state, $details);

  // return the form
*/
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
 * @ingroup tripal_ontology_term_submission
 */
function chado_ontology_term_submission_validate($node, $form, &$form_state) {

  // We only want to validate when the node is saved.
  // Since this validate can be called on AJAX and Deletion of the node
  // we need to make this check to ensure queries are not executed
  // without the proper values.
  if(property_exists($node, "op") and $node->op != 'Save') {
    return;
  }

  // we are syncing if we do not have a node ID but we do have a ontology_term_submission_id. We
  // don't need to validate during syncing so just skip it.
  if (!property_exists($node, 'nid') and property_exists($node, 'ontology_term_submission_id') and $node->ontology_term_submission_id != 0) {
    return;
  }

  // be sure to always trim text fields
  $node->name   = property_exists($node, 'name') ? trim($node->name) : '';

  // Validating for an update. If the 'nid' property is present in the node then
  // this is an update and validation can be different for updates
  if (property_exists($node, 'nid')) {

    // make sure the ontology_term_submission type is an allowed term
    //$type_cv = tripal_get_default_cv('ontology_term_submission', 'type_id');
    //$type = tripal_get_cvterm(array(
     // 'name' => $node->ontology_term_submission_type,
     // 'cv_id' => $type_cv->cv_id,
   // ));
   // if (!$type) {
    //  form_set_error('ontology_term_submission_type', t("The ontology_term_submission type is not a valid name from the Sequence Ontology."));
   // }

    // TODO: also we should check that the unique constraint is not invalidated
    // by changing either the type_id, organism_id or uniquename.
  }
  // Validating for an insert
  else {
    // make sure the ontology_term_submission type is an allowed term
    //$type_cv = tripal_get_default_cv('ontology_term_submission', 'type_id');
   // $type = tripal_get_cvterm(array(
     // 'name' => $node->ontology_term_submission_type,
    //  'cv_id' => $type_cv->cv_id,
   // ));
    //if (!$type) {
   //   form_set_error('ontology_term_submission_type', t("The ontology_term_submission type is not a valid name from the Sequence Ontology."));
   // }

    // TODO: also we should check that the unique constraint doesn't already exist
  }
}

/**
 *  Implementation of hook_insert(). This function is called after the node is
 * inserted into the database. We need it so that we can insert appropriate
 * fields as provided by the user into the database. And so that we can link the
 * new Drupal node to the data in Chado via the chado_ontology_term_submission linking table. We
 * can get to this function also during "syncing".
 * With syncing, however, the data already exists in Chado and we do not want
 * to try to re-add it. But we do need to add an entry to the chado_ontology_term_submission
 * table to link the Drupal node with the data in the 'ontology_term_submission' table of Chado.
 *
 *  This function is not required if the hook_node_info() does not define
 *  any custom node types.
 *
 * @ingroup tripal_ontology_term_submission
 */
function chado_ontology_term_submission_insert($node) {

  $ontology_term_submission_id = '';

  // if there is an ontology_term_submission_id in the $node object then this must be a sync so
  // we can skip adding the ontology_term_submission as it is already there, although we do need
  // to proceed with insertion into the chado/drupal linking table.
  if (!property_exists($node, 'ontology_term_submission_id')) {

    // be sure to always trim text fields
    $node->name   = trim($node->name);
    $node->description  = trim($node->description['value']);

    // get the ontology_term_submission type record
    $type_cv = tripal_get_default_cv('ontology_term_submission', 'type_id');
    $type = tripal_get_cvterm(array(
        'name' => $node->ontology_term_submission_type,
        'cv_id' => $type_cv->cv_id,
    ));

    // perform the insert using the chado_insert_record function();
    $values = array(
      'name' => $node->name,
      'defintion' => $node->defintion,
      'type_id' => $type->cvterm_id,
      'defintion_ref' => $node->defintion_ref,
      'term_xref' => $node->term_xref,
      'seeAlso_url' => $node->seeAlso_url,
      'contact_name' => $node->contact_name,
      'contact_email' => $node->contact_email,
      'comments' => $node->comments,
      'parent_cvterm_id_1' => $node->parent_cvterm_id_1,
      'relation_cvterm_id_1' => $node->relation_cvterm_id_1,
      'parent_cvterm_id_2' => $node->parent_cvterm_id_2,
      'relation_cvterm_id_2' => $node->relation_cvterm_id_2,
      'parent_cvterm_id_3' => $node->parent_cvterm_id_3,
      'relation_cvterm_id_2' => $node->relation_cvterm_id_2,
    );
    $ontology_term_submission = chado_insert_record('ontology_term_submission', $values);
    if (!$ontology_term_submission) {
      drupal_set_message(t('Unable to add ontology_term_submission.'), 'warning');
      tripal_report_error('tripal_ontology_term_submission', TRIPAL_WARNING, 'Insert ontology_term_submission: Unable to create ontology_term_submission where values: %values',
        array('%values' => print_r($values, TRUE)));
      return;
    }

    // get the ontology_term_submission_id for linking Drupal node with Chado data
    $ontology_term_submission_id = $ontology_term_submission['ontology_term_submission_id'];

    // Only add to other Chado tables if the base record was inserted properly
    if ($ontology_term_submission_id > 0) {

/*      // If you implemented the properties form in chado_ontology_term_submission_form then you
      // need to handle inserting these properties into your Chado prop table.
      $details = array(
        // the name of the prop table
        'property_table'   => 'ontology_term_submissionprop',
        // the name of your Chado base table
        'base_table'       => 'ontology_term_submission',
        // the name of the key in your base table
        'foreignkey_name'  => 'ontology_term_submission_id',
        // the value of the ontology_term_submission_id key
        'foreignkey_value' => $ontology_term_submission_id
      );
      chado_update_node_form_properties($node, $details);

      // If you implemented the dbxrefs form in chado_ontology_term_submission_form then you need
      // to handle inserting these database references into your Chado _dbxref
      // table.
      $details = array(
        // the name of your _dbxref table
        'linking_table'    => 'ontology_term_submission_dbxref',
        // the name of the key in your base table
        'foreignkey_name'  => 'ontology_term_submission_id',
        // the value of the ontology_term_submission_id key
        'foreignkey_value' => $ontology_term_submission_id
      );
      chado_update_node_form_dbxrefs($node, $details);

      // If you implemented the relationships form in chado_ontology_term_submission_form then
      // you need to handle inserting these relationships into your Chado
      // _relationship table.
      $details = array(
        // name of the _relationship table
        'relationship_table' => 'ontology_term_submission_relationship',
        // value of the ontology_term_submission_id key
        'foreignkey_value' => $ontology_term_submission_id
      );
      chado_update_node_form_relationships($node, $details);
 */ 
   }
  }
  else {
    // the node has an ontology_term_submission_id so get it for linking Drupal node with Chado
    // data
    $ontology_term_submission_id = $node->ontology_term_submission_id;
  }

  // Make sure the entry for this ontology_term_submission doesn't already exist in the
  // chado_ontology_term_submission table if it doesn't exist then we want to add it.
   $check_org_id = chado_get_id_from_nid('ontology_term_submission', $node->nid);
  if (!$check_org_id) {
    $record = new stdClass();
    $record->nid = $node->nid;
    $record->vid = $node->vid;
    $record->ontology_term_submission_id = $ontology_term_submission_id;
    drupal_write_record('chado_ontology_term_submission', $record);
  }
}

/**
 * Implementation of hook_update(). This function runs after the node has been
 * inserted into the Drupal schema and allows us to update the record in Chado.
 *
 * This function is not required if the hook_node_info() does not define any
 * custom node types.
 *
 * @ingroup tripal_ontology_term_submission
 */
function chado_ontology_term_submission_update($node) {
  // be sure to always trim text fields
  $node->name   = trim($node->name);
  $node->definition  = trim($node->definition['value']);

  // use the chado_update_record() function to update the record
  $match = array(
    'ontology_term_submission_id' => $ontology_term_submission_id,
  );
  $values = array(
    'name' => $node->name,
  );
  $options = array('return_record' => TRUE);
  $status = chado_update_record('ontology_term_submission', $match, $values, $options);

  if (!$status) {
    drupal_set_message(t('Unable to update ontology_term_submission.'), 'warning');
    tripal_report_error('tripal_ontology_term_submission', TRIPAL_WARNING, 'Update ontology_term_submission: Unable to update ontology_term_submission where values: %values',
      array('%values' => print_r($values, TRUE)));
  }

/*  // If you implemented the properties form in chado_ontology_term_submission_form then you need
  // to handle updating these properties into your Chado prop table.
  $details = array(
    'property_table' => 'ontology_term_submissionprop', // the name of the prop table
    'base_table' => 'ontology_term_submission',         // the name of your Chado base table
    'foreignkey_name' => 'ontology_term_submission_id', // the name of the key in your base table
    'foreignkey_value' => $ontology_term_submission_id  // the value of the ontology_term_submission_id key
  );
  chado_update_node_form_properties($node, $details);

  // If you implemented the dbxrefs form in chado_ontology_term_submission_form then you need to
  // handle updating these database references into your Chado _dbxref table.
  $details = array(
    'linking_table' => 'ontology_term_submission_dbxref', // the name of your _dbxref table
    'foreignkey_name' => 'ontology_term_submission_id',   // the name of the key in your base table
    'foreignkey_value' => $ontology_term_submission_id    // the value of the ontology_term_submission_id key
  );
  chado_update_node_form_dbxrefs($node, $details);

  // If you implemented the relationships form in chado_ontology_term_submission_form then you
  // need to handle updating these relationships into your Chado _relationship
  // table.
  $details = array(
    // name of the _relationship table
    'relationship_table' => 'ontology_term_submission_relationship',
    // value of the ontology_term_submission_id key
    'foreignkey_value' => $ontology_term_submission_id
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
 * @ingroup tripal_ontology_term_submission
 */
function chado_ontology_term_submission_delete($node) {

  // get the ontology_term_submission id from the node
  $ontology_term_submission_id  = chado_get_id_from_nid('ontology_term_submission', $node->nid);

  // if we don't have a ontology_term_submission id for this node then this isn't a node of type
  // chado_ontology_term_submission or the entry in the chado_ontology_term_submission table was lost.
  if (!$ontology_term_submission_id) {
    return;
  }

  // remove the entry in the chado_exapmle table linking the deleted
  // Drupal node with the data in Chado
  $sql_del = "DELETE FROM {chado_ontology_term_submission} WHERE nid = :nid AND vid = :vid";
  db_query($sql_del, array(':nid' => $node->nid, ':vid' => $node->vid));

  // Remove data from ontology_term_submission tables of Chado database. This will
  // cause a cascade delete and remove all data in referencing tables
  // for this ontology_term_submission
  chado_query("DELETE FROM {ontology_term_submission} WHERE ontology_term_submission_id = :ontology_term_submission_id", array(':ontology_term_submission_id' => $ontology_term_submission_id));

  // inform the user that the data was deleted
  drupal_set_message(t("The ontology_term_submission and all associated data were removed from Chado"));

}

/**
 * Implementation of hook_load(). This function is necessary to load into the
 * $node object the fields of the table form Chado. For ontology_term_submission for the ontology_term_submission
 * table, the chado_ontology_term_submission_load() function adds in a ontology_term_submission object which
 * contains all of the fields and sub objects for data in tables with foreign
 * key relationships.
 *
 * This function is not required if the hook_node_info() does not define any
 * custom node types.
 *
 * @ingroup tripal_ontology_term_submission
 */
function chado_ontology_term_submission_load($nodes) {

  // EXPLANATION: when displaying or node or accessing the node in a template
  // we need the data from Chado. This function finds the record in Chado that
  // this node belongs to and adds the record.

  // there may be multiple nodes that get passed in so we have to iterate
  // through them all
  foreach ($nodes as $nid => $node) {
    // find the ontology_term_submission and add in the details
    $ontology_term_submission_id = chado_get_id_from_nid('ontology_term_submission', $nid);

    // if the nid does not have a matching record then skip this node.
    // this can happen with orphaned nodes.
    if (!$ontology_term_submission_id) {
      continue;
    }

    // build the ontology_term_submission variable by using the chado_generate_var() function
    $values = array('ontology_term_submission_id' => $ontology_term_submission_id);
    $ontology_term_submission = chado_generate_var('ontology_term_submission', $values);

    // for fields in the table that are of type 'text' you may want to include
    // those by default, the chado_generate_var does not include text fields as
    // they may be very large and including a large text field can slow the page
    // load.
    // If you know a text field will never be large and it is important for the
    // other functions that will see the node to have access to a field you can
    // include it here using the chado_expand_var() function. In most
    // cases it is probably best to let the end-user decide if text fields
    // should be included by using this function in the templates.
    $ontology_term_submission = chado_expand_var($ontology_term_submission, 'field', 'ontology_term_submission.definition');
    $ontology_term_submission = chado_expand_var($ontology_term_submission, 'field', 'ontology_term_submission.defintion_ref');
    $ontology_term_submission = chado_expand_var($ontology_term_submission, 'field', 'ontology_term_submission.term_xref');
    $ontology_term_submission = chado_expand_var($ontology_term_submission, 'field', 'ontology_term_submission.seeAlso_url');
    $ontology_term_submission = chado_expand_var($ontology_term_submission, 'field', 'ontology_term_submission.contact_name');
    $ontology_term_submission = chado_expand_var($ontology_term_submission, 'field', 'ontology_term_submission.contact_email');
    $ontology_term_submission = chado_expand_var($ontology_term_submission, 'field', 'ontology_term_submission.comments');

    // add the new ontology_term_submission object to this node.
    $nodes[$nid]->ontology_term_submission = $ontology_term_submission;

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
 * @ingroup tripal_ontology_term_submission
 */
function tripal_ontology_term_submission_node_presave($node) {

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
    case 'chado_ontology_term_submission':
      // for a form submission the 'uniquename' field will be set,
      // for a sync, we must pull from the ontology_term_submission object
      if (property_exists($node, 'name')) {
        // set the title
        $node->title = "Term Submission:$node->name by $node->contact_name";
      }
      else if (property_exists($node, 'ontology_term_submission')) {
        $node->title = "Term Submission:$node->ontology_term_submission->name by $node->ontology_term_submission->contact_name";
      }
      break;
  }
}

/**
 * Implementation of hook node_insert().
 *
 * Performs actions after any node has been inserted.
 *
 * @ingroup tripal_ontology_term_submission
 */
function tripal_ontology_term_submission_node_insert($node) {

  // EXPLANATION: This function is used after any a node is inserted into the
  // database. It is different from the hook_insert() function above in that it
  // is called after any node is saved, regardless of it's type. This function
  // is useful for making changes to the database after a node is inserted.
  // An ontology_term_submission comes from the tripal_feature module where the URL alias of a
  // node cannot be set in the hook_insert() function. Therefore the
  // tripal_feature module uses this function to set the URL path of a newly
  // inserted ontology_term_submission node.
  //
  // This function is not required. You probably won't need it if you don't
  // define a custom node type in the hook_node_info() function. But it is node
  // type agnostic, so you can use this function to do any activity after insert
  // of any node.

  // the Ontology Term Submission code below will set the URL path after inserting. We do it here
  // because we do not know the ontology_term_submission_id in the pre-save and cannot do it in
  // the hook_insert()
  switch ($node->type) {
    case 'chado_ontology_term_submission':

      // find the ontology_term_submission and add in the details
      $ontology_term_submission_id = chado_get_id_from_nid('ontology_term_submission', $node->nid);

      // build the ontology_term_submission variable by using the chado_generate_var() function
      $values = array('ontology_term_submission_id' => $ontology_term_submission_id);
      $ontology_term_submission = chado_generate_var('ontology_term_submission', $values);
      $node->ontology_term_submission = $ontology_term_submission;


      // EXPLANATION: You can allow the site admin to customize the
      // title and URL of your node.  The 'Chado Node: Title & Path API'
      // contains two functions that can be called to generate the title and
      // URL based a schema provided by the site admin. These functions are
      // named chado_get_node_title() and chado_set_node_url().  These
      // functions use a string of tokens to build the URL and titles and the
      // site admin has the ability to set these tokens.  There are
      // form elements made available in the tripal_ontology_term_submission_admin() function
      // that allow the admin to set these tokens.  The default token string
      // is provided to Tripal using two hook functions, and are found below.
      // These are: chado_exmaple_chado_node_default_title() and
      // chado_ontology_term_submission_chdo_node_default_url().

      // Set the Title and URL for this node.
      $ontology_term_submission->title = chado_get_node_title($node);
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
function tripal_ontology_term_submission_node_update($node) {

  // EXPLANATION: This function is used after any a node is updated in the
  // database. It is different from the hook_update() function above in that it
  // is called after any node is updated, regardless of it's type.
  // An ontology_term_submission comes from the tripal_feature module where the URL alias of a
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
    case 'chado_ontology_term_submission':

      // If your module is using the Chado Node: Title & Path API to allow
      // custom titles for your node type. Every time you want the title of the
      // node, you need to use the following API function:
      $ontology_term_submission->title = chado_get_node_title($node);

      // set the URL for this ontology_term_submission page
      // see the code in the tripal_feature/includes/tripal_feature.chado_node.inc
      // file in the function tripal_feature_node_insert for an ontology_term_submission of how
      // that module sets the URL. It uses a configuration file to allow the
      // user to dynamically build a URL schema and then uses that schema to
      // generate a URL string.
      break;
  }
}
/**
 * Implementation of hook_node_view().
 *
 * @ingroup tripal_ontology_term_submission
 */
function tripal_ontology_term_submission_node_view($node, $view_mode, $langcode) {

  // EXPLANATION: This function defines the content "blocks" that appear when
  // the node is displayed. It is node type agnostic so we can add content to
  // any node type. So, we use this function to add the content from all of our
  // theme templates onto our new node type. We will also use this function to
  // add content to other node types.

  switch ($node->type) {
    case 'chado_ontology_term_submission':
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
        // shown when the ontology_term_submission node type is first displayed.
        // If you are using the default Tripal node template, then you should
        // also set two additional items in each array:  tripal_toc_id and
        // tripal_toc_title. The tripal_tock_id should be a single unique
        // world that is used to reference the template. This ID is used for
        // constructing URLs for the content. The tripal_toc_title contains
        // the title that should appear in the table of contents for this
        // content. You should only set the '#weight' element for the base
        // template (or Overview) to ensure that it appears at the top of the
        // list. Otherwise items are sorted alphabetically.
        $node->content['tripal_ontology_term_submission_base'] = array(
          '#theme' => 'tripal_ontology_term_submission_base',
          '#node' => $node,
          '#tripal_toc_id'    => 'base',
          '#tripal_toc_title' => 'Overview',
          '#weight' => -100,
        );
/*        // we can add other templates as well for properties, publications,
        // dbxrefs, etc...
        $node->content['tripal_ontology_term_submission_properties'] = array(
          '#theme' => 'tripal_ontology_term_submission_properties',
          '#node' => $node,
          '#tripal_toc_id'    => 'properties',
          '#tripal_toc_title' => 'Properties',
        );
        $node->content['tripal_ontology_term_submission_references'] = array(
          '#theme' => 'tripal_ontology_term_submission_references',
          '#node' => $node,
          '#tripal_toc_id'    => 'references',
          '#tripal_toc_title' => 'Cross References',
        );
        $node->content['tripal_ontology_term_submission_relationships'] = array(
          '#theme' => 'tripal_ontology_term_submission_relationships',
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
        $node->content['tripal_ontology_term_submission_teaser'] = array(
          '#theme' => 'tripal_ontology_term_submission_teaser',
          '#node' => $node,
        );
      }
      break;
    // you can add custom content to any node type by adding content to the node
    // in the same way as above.
    case 'chado_organism':
      if ($view_mode == 'full') {
        $node->content['tripal_organism_ontology_term_submissions'] = array(
          '#theme' => 'tripal_organism_ontology_term_submissions',
          '#node' => $node,
          '#tripal_toc_id'    => 'ontology_term_submissions',
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
 * Chado ontology_term_submission nodes based on chado fields.
 */
function chado_ontology_term_submission_chado_node_default_title_format() {
  //return '[ontology_term_submission.uniquename] ([ontology_term_submission.type_id>cvterm.name]) [ontology_term_submission.organism_id>organism.genus] [ontology_term_submission.organism_id>organism.species]';
  return '[ontology_term_submission.name] ([ontology_term_submission.type_id>cvterm.name]) by [ontology_term_submission.contact_name]';
}

/**
 * Implements hook_chado_node_default_url_format().
 *
 * Designates a default URL format for ontology_term_submission nodes.
 */
function chado_ontology_term_submission_chado_node_default_url_format() {
  //return '/ontology_term_submission/[ontology_term_submission.organism_id>organism.genus]/[ontology_term_submission.organism_id>organism.species]/[ontology_term_submission.type_id>cvterm.name]/[ontology_term_submission.uniquename]';
  return '/ontology_term_submission/ontology_term_submission.type_id>cv.name]/[ontology_term_submission.name]';
}
