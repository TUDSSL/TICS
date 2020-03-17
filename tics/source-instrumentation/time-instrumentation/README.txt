Paper 3.2.3

--------------------------------------------------------------------------------
@expires_after=1s

Attach a timeout to a variable, if this variable is an array, the time value
holds for the whole array. Not per element.
The variable is encapsulated within a structure that maintains the time
information.
The time starts when a time assign (@=) is done.
The time can be specified in s (seconds) or ms (milliseconds) and has to be a
whole number (integer).
This has to be declared at compile-time. The expiration time can not be modified
at runtime.

--------------------------------------------------------------------------------
@=
A 'time assign'. This assignments does not only assign the value to the time
variable, but also "starts" the time. So the current time is registered and when
an @expires block is used, the variable is expired if the "expires_after" value
plus the time of the "@=" exceeds the current time at the @expires block.

--------------------------------------------------------------------------------
@expires{}
@catch{}

Set checkpoint at the start, disable checkpoints.
When expires fires, do a restore.
At the expires/catch entry, check if the value is expired
If so, do the catch block. If not, do the expires block.
If there is no @catch block, and the value is expired, skip the @expires block.
This can retry multiple times (when a reboot happens) until the data expires.
This has to be declared at compile-time. The expiration time can not be modified
at runtime.

--------------------------------------------------------------------------------
@timely{}
else{}

Works like an @expires without a variable. Provide a time after which the block
is not taken. This can retry multiple times (when a reboot happens) until the
data expires.
The time can be specified in s (seconds) or ms (milliseconds) and has to be a
whole number (integer).
This has to be declared at compile-time. The expiration time can not be modified
at runtime.

--------------------------------------------------------------------------------
NOTE: If an expired value is too long, a boot loop can happen until the variable
is expired. This is a concious decision, as this is what the programmer
instructed to happen.
