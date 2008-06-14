class RainbowController < ApplicationController
  layout 'rainbow'

  def index
    @side_partial = 'side'
  end

  def example
    @side_partial = 'side'
  end

  def handle_unknown_request
    redirect_to(:action => 'index')
  end
end
