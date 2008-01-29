class RootController < ApplicationController
	layout 'main'
	
	def index
		@top_partial = 'download'
	end
	
	def handle_unknown_request
		redirect_to(:action => 'index')
	end
end
