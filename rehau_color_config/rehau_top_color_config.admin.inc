<?php

function rehau_top_color_config_form($form, &$form_state){
  $colors = rehau_get_active_colors();

  $form['colours'] = array(
    '#type'=>'container',
    '#tree'=>true,
  );
  foreach($colors AS $color) {

    $form['colours'][$color['colour_code']] = array(
      '#type'=>'container',
      '#tree'=>true,
    );
    $form['colours'][$color['colour_code']]['colour_state'] = array(
      '#type' => 'checkbox',
      '#default_value' => !empty($form_state['values']['colours'][$color['colour_code']]['colour_state']) ? $form_state['values']['colours'][$color['colour_code']]['colour_state'] : $color['colour_state'],
    );
    $form['colours'][$color['colour_code']]['colour_code'] = array(
      '#markup' => $color['colour_code'],
    );
    $form['colours'][$color['colour_code']]['colour_title'] = array(
      '#markup' => $color['colour_text'],
    );

  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function rehau_top_color_config_form_validate(&$form, &$form_state) {

  $i = 0;
  foreach($form_state['values']['colours'] AS $key => $vals) {
    if($vals['colour_state'] == 1){
      $i++;
    }
  }
  if($i<6 || $i>6){
    form_set_error('', t('Select 6 Top colours.'));
  }
}

function rehau_top_color_config_form_submit($form, &$form_state) {
  global $user;
  $logged_in_user = $user->uid;
  $result = make_colour_code_key(rehau_get_top_colours($logged_in_user), FALSE);
  foreach($result as $code => $val){
    $available = FALSE;
    foreach($form_state['values']['colours'] AS $key => $vals) {
      if($code == $key){
        $available = TRUE;
      }
      else{
        $available = FALSE;
      }
    }
    if(!$available){
      db_delete('winpro_api_top_colours')
        ->condition('colour_code', $code)
        ->execute();
    }
  }
  foreach($form_state['values']['colours'] AS $key => $vals) {
    $colour = rehau_admin_get_color_info($key);
    if($vals['colour_state'] == 1){
      if($result[$key]) {
        $result['colour_text'] = $colour['colour_text'];
        drupal_write_record('winpro_api_top_colours',$result[$key],$result[$key]['id']);
      }
      else{
        $add_row = db_insert('winpro_api_top_colours')
          ->fields(array(
            'uid' => $logged_in_user,
            'colour_code' => $key,
            'colour_text' => $colour['colour_text'],
            'colour_code_hex' => $colour['colour_code_hex'],
          ))
          ->execute();
      }
    }
  }
  drupal_set_message(t('The changes are saved.'));
}
