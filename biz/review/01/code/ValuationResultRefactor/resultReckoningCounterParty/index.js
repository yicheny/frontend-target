import { createView } from '../widgets/components'
import Contract from './Contract'
import Position from './Position'

export default createView({
  TabContent1: Position,
  TabContent2: Contract,
})
