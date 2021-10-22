package Time::Out ;
@ISA = qw(Exporter) ;
@EXPORT_OK = qw(timeout) ;

use strict ;
use Exporter ;
use Carp ;


BEGIN {
	if (Time::HiRes->can('alarm')){
		Time::HiRes->import('alarm') ;
	}
	if (Time::HiRes->can('time')){
		Time::HiRes->import('time') ;
	}
}


$Time::Out::VERSION = '0.11' ;


sub timeout($@){
	my $secs = shift ;
	carp("Timeout value evaluates to 0: no timeout will be set") if ! $secs ;
	my $code = pop ;
	usage() unless ((defined($code))&&(UNIVERSAL::isa($code, 'CODE'))) ;
	my @other_args = @_ ;

	# Disable any pending alarms.
	my $prev_alarm = alarm(0) ;
	my $prev_time = time() ;
	my $dollar_at = undef ;
	my @ret = () ;
	{
		# Disable alarm to prevent possible race condition between end of eval and execution of alarm(0) after eval.
		local $SIG{ALRM} = sub {} ; 
		@ret = eval {
			local $SIG{ALRM} = sub { die $code } ;
			if (($prev_alarm)&&($prev_alarm < $secs)){
				# A shorter alarm was pending, let's use it instead.
				alarm($prev_alarm) ;
			}
			else {
				alarm($secs) ;
			}
			my @ret = $code->(@other_args) ;
			alarm(0) ;
			@ret ;
		} ;
		alarm(0) ;	
		$dollar_at = $@ ;
	}

	my $new_time = time() ;
    my $new_alarm = $prev_alarm - ($new_time - $prev_time) ;
	if ($new_alarm > 0){
		# Rearm old alarm with remaining time.
		alarm($new_alarm) ;
	}
	elsif ($prev_alarm){
		# Old alarm has already expired.
		kill 'ALRM', $$ ;
	}

	if ($dollar_at){
		if ((ref($dollar_at))&&($dollar_at eq $code)){
			$@ = "timeout" ;
		}
		else {
			if (! ref($dollar_at)){
				chomp($dollar_at) ;
				die("$dollar_at\n") ;
			}
			else {
				croak $dollar_at ;
			}
		}
	}

	return wantarray ? @ret : $ret[0] ;
}


sub usage {
	croak("Usage: timeout \$nb_secs => sub {\n  #code\n} ;\n") ;
}



1 ;
