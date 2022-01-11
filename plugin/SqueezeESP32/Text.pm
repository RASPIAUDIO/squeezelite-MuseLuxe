package Plugins::SqueezeESP32::Text;

use strict;

use base qw(Slim::Display::Text);

# we don't want the special Noritake codes
sub vfdmodel {
	return 'squeezeslave';
}

1;