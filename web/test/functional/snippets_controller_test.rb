require 'test_helper'

class SnippetsControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:snippets)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_snippet
    assert_difference('Snippet.count') do
      post :create, :snippet => { }
    end

    assert_redirected_to snippet_path(assigns(:snippet))
  end

  def test_should_show_snippet
    get :show, :id => snippets(:one).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => snippets(:one).id
    assert_response :success
  end

  def test_should_update_snippet
    put :update, :id => snippets(:one).id, :snippet => { }
    assert_redirected_to snippet_path(assigns(:snippet))
  end

  def test_should_destroy_snippet
    assert_difference('Snippet.count', -1) do
      delete :destroy, :id => snippets(:one).id
    end

    assert_redirected_to snippets_path
  end
end
