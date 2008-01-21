class RootController < ApplicationController
	layout 'main'
	
	def index
		@top_partial = 'download'
	end
end
