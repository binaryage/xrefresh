class RootController < ApplicationController
	layout 'main', :except => ['digg']
	
	def index
		@page = 'home';
		@top_partial = 'download'
		@side_partial = 'side'
	end

	def about
		@page = 'about';
		@side_partial = 'side_about'
  end

  def digg
  end
	
	def handle_unknown_request
		redirect_to(:action => 'index')
	end
end
