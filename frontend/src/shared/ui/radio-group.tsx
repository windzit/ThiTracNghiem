import * as React from "react"
import { Root, Item, Indicator } from "@radix-ui/react-radio-group"
import { Circle } from "lucide-react"
import { cn } from "@/shared/lib/utils"

const RadioGroup = React.forwardRef<
  React.ElementRef<typeof Root>,
  React.ComponentPropsWithoutRef<typeof Root>
>(({ className, ...props }, ref) => {
  return <Root className={cn("grid gap-2", className)} {...props} ref={ref} />
})
RadioGroup.displayName = Root.displayName

const RadioGroupItem = React.forwardRef<
  React.ElementRef<typeof Item>,
  React.ComponentPropsWithoutRef<typeof Item>
>(({ className, ...props }, ref) => {
  return (
    <Item
      ref={ref}
      className={cn(
        "aspect-square h-4 w-4 rounded-full border border-gray-400 text-[#D9272B] shadow focus:outline-none focus-visible:ring-1 focus-visible:ring-ring disabled:cursor-not-allowed disabled:opacity-50 data-[state=checked]:border-[#D9272B] data-[state=checked]:text-[#D9272B]",
        className
      )}
      {...props}
    >
      <Indicator className="flex items-center justify-center">
        <Circle className="h-2.5 w-2.5 fill-current text-current" />
      </Indicator>
    </Item>
  )
})
RadioGroupItem.displayName = Item.displayName

export { RadioGroup, RadioGroupItem }