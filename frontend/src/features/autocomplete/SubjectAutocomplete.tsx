import { useState, useRef, useEffect, useMemo } from "react"
import { ChevronDown, Check, X } from "lucide-react"
import type { Subject } from "@/shared/types"

interface SubjectAutocompleteProps {
  items: Subject[]
  value: Subject | null
  onChange: (subject: Subject | null) => void
  placeholder?: string
  className?: string
}

// Remove diacritics for fuzzy search
const removeDiacritics = (str: string): string =>
  str.normalize("NFD").replace(/[\u0300-\u036f]/g, "")

// Highlight matching text query
function HighlightText({ text, query }: { text: string; query: string }) {
  if (!query) return <>{text}</>
  const normalizedText = removeDiacritics(text.toLowerCase())
  const normalizedQuery = removeDiacritics(query.toLowerCase())
  const idx = normalizedText.indexOf(normalizedQuery)
  if (idx === -1) return <>{text}</>
  return (
    <>
      {text.slice(0, idx)}
      <span className="bg-yellow-200 font-semibold">{text.slice(idx, idx + query.length)}</span>
      {text.slice(idx + query.length)}
    </>
  )
}

export function SubjectAutocomplete({
  items,
  value,
  onChange,
  placeholder = "Tìm theo mã môn, tên môn...",
  className = "",
}: SubjectAutocompleteProps) {
  const [isOpen, setIsOpen] = useState(false)
  const [query, setQuery] = useState("")
  const [highlightedIndex, setHighlightedIndex] = useState(-1)
  const inputRef = useRef<HTMLInputElement>(null)
  const listRef = useRef<HTMLDivElement>(null)

  // Filter items in real-time by code (MAMH) or name (TENMH)
  const filteredItems = useMemo(() => {
    if (!query) return items
    const normalizedQuery = removeDiacritics(query.toLowerCase())
    return items.filter((item) => {
      const codeMatch = removeDiacritics(item.code.toLowerCase()).includes(normalizedQuery)
      const nameMatch = removeDiacritics(item.name.toLowerCase()).includes(normalizedQuery)
      return codeMatch || nameMatch
    })
  }, [items, query])

  // Close dropdown when clicking outside
  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (listRef.current && !listRef.current.contains(event.target as Node)) {
        setIsOpen(false)
        setQuery("")
      }
    }
    document.addEventListener("mousedown", handleClickOutside)
    return () => document.removeEventListener("mousedown", handleClickOutside)
  }, [])

  const handleSelect = (item: Subject) => {
    onChange(item)
    setQuery("")
    setIsOpen(false)
    inputRef.current?.blur()
  }

  const handleClear = (e: React.MouseEvent) => {
    e.stopPropagation()
    onChange(null)
    setQuery("")
    setIsOpen(false)
  }

  const handleKeyDown = (e: React.KeyboardEvent) => {
    if (!isOpen) {
      if (e.key === "Enter" || e.key === "ArrowDown") {
        setIsOpen(true)
        e.preventDefault()
      }
      return
    }

    switch (e.key) {
      case "ArrowDown":
        e.preventDefault()
        setHighlightedIndex((prev) => (prev < filteredItems.length - 1 ? prev + 1 : 0))
        break
      case "ArrowUp":
        e.preventDefault()
        setHighlightedIndex((prev) => (prev > 0 ? prev - 1 : filteredItems.length - 1))
        break
      case "Enter":
        e.preventDefault()
        if (highlightedIndex >= 0 && highlightedIndex < filteredItems.length) {
          handleSelect(filteredItems[highlightedIndex])
        }
        break
      case "Escape":
        e.preventDefault()
        setIsOpen(false)
        setQuery("")
        break
    }
  }

  const handleInputChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setQuery(e.target.value)
    setIsOpen(true)
    setHighlightedIndex(-1)
    if (value) {
      onChange(null)
    }
  }

  const handleFocus = () => {
    setIsOpen(true)
  }

  const inputValue = isOpen ? query : value ? `${value.code} - ${value.name}` : query

  return (
    <div className={`relative ${className}`} ref={listRef}>
      <div className="relative">
        <input
          ref={inputRef}
          type="text"
          className="w-full h-10 pl-3 pr-8 rounded-lg border border-gray-200 text-sm text-gray-700 placeholder:text-gray-400 focus:outline-none focus:ring-2 focus:ring-[#D9272B]/20 focus:border-[#D9272B]"
          placeholder={value ? `${value.code} - ${value.name}` : placeholder}
          value={inputValue}
          onChange={handleInputChange}
          onKeyDown={handleKeyDown}
          onFocus={handleFocus}
        />
        <div className="absolute right-2.5 top-1/2 -translate-y-1/2 flex items-center gap-1">
          {(value || query) && (
            <button
              type="button"
              onClick={handleClear}
              className="p-0.5 rounded-full hover:bg-gray-100 text-gray-400 hover:text-gray-600 transition-colors"
              title="Xóa lựa chọn"
            >
              <X className="h-3.5 w-3.5" />
            </button>
          )}
          <ChevronDown
            className={`h-4 w-4 text-gray-400 transition-transform pointer-events-none ${
              isOpen ? "rotate-180" : ""
            }`}
          />
        </div>
      </div>

      {isOpen && (
        <div className="absolute z-50 w-full mt-1 bg-white rounded-lg border border-gray-200 shadow-lg max-h-60 overflow-y-auto">
          {filteredItems.length === 0 ? (
            <div className="px-3 py-2.5 text-sm text-gray-400">Không tìm thấy môn học.</div>
          ) : (
            filteredItems.map((item, index) => {
              const fullText = `${item.code} - ${item.name}`
              const isSelected = value?.id === item.id || value?.code === item.code
              return (
                <button
                  key={item.id || item.code}
                  type="button"
                  className={`w-full flex items-center justify-between px-3 py-2.5 text-left text-sm transition-colors ${
                    index === highlightedIndex ? "bg-gray-100" : "hover:bg-gray-50"
                  } ${isSelected ? "bg-blue-50 text-blue-700 font-medium" : "text-gray-900"}`}
                  onClick={() => handleSelect(item)}
                  onMouseEnter={() => setHighlightedIndex(index)}
                >
                  <div className="truncate pr-2">
                    <HighlightText text={fullText} query={query} />
                  </div>
                  {isSelected && (
                    <Check className="h-4 w-4 text-blue-600 flex-shrink-0" />
                  )}
                </button>
              )
            })
          )}
        </div>
      )}
    </div>
  )
}