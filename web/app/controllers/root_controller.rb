class RootController < ApplicationController
	layout 'main'
	
	def index
		@top_partial = 'download'
		@side_partial = 'side'
	end

	def about
		@side_partial = 'side'
	end
	
	def handle_unknown_request
		redirect_to(:action => 'index')
	end
end
